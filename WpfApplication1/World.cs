using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool
{
    class World
    {
        public enum TileType
        {
            Water,
            Plain,
            Forest,
            Mountain
        }

        public List<List<TileType>> myWorldData;
        private Random randomizer;

        public World() { randomizer = new Random(); }
        public void Generate(WorldParameters aParemeters)
        {
            int width = (int)aParemeters.Data.width;
            int height = (int)aParemeters.Data.height;

            myWorldData = new List<List<TileType>>();

            // Fill map with water
            for (int y = 0; y < height; ++y)
            {
                myWorldData.Add(new List<TileType>());
                for (int x = 0; x < width; ++x)
                {
                    myWorldData[y].Add(TileType.Water);
                }
            }

            // Add land
            float targetLandPercentage = aParemeters.Data.landPercentage;
            float landIncrementConstant = 1.0f / (width * height);
            float islandModifier = 1.1f - aParemeters.Data.waterBodyBias;
            float volume = width * height;
            Random randomizer = new Random();
            List<List<TileType>> buffer = myWorldData;

            // Plain
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if (randomizer.Next(0, 101) < 40 + 20.0f * targetLandPercentage)
                    {
                        buffer[y][x] = TileType.Plain;
                    }
                }
            }

            for (int i = 0; i < 5; ++i)
            {
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        if (buffer[y][x] == TileType.Water && GetAliveNeighbourCount(TileType.Plain, buffer, y, x) >= 5)
                        {
                            buffer[y][x] = TileType.Plain;
                        }
                        if (buffer[y][x] == TileType.Plain && GetAliveNeighbourCount(TileType.Plain, buffer, y, x) < 4)
                        {
                            buffer[y][x] = TileType.Water;
                        }
                    }
                }
            }

            // Forest
            float targetForestPercentage = aParemeters.Data.forestPercentage;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if ( buffer[y][x] == TileType.Plain && randomizer.Next(0, 101) < 40 + 40.0f * targetForestPercentage)
                    {
                        buffer[y][x] = TileType.Forest;
                    }
                }
            }

            for (int i = 0; i < 5; ++i)
            {
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        if (buffer[y][x] == TileType.Plain && GetAliveNeighbourCount(TileType.Forest, buffer, y, x) >= 5)
                        {
                            buffer[y][x] = TileType.Forest;
                        }
                        if (buffer[y][x] == TileType.Forest && GetAliveNeighbourCount(TileType.Forest, buffer, y, x) < 4)
                        {
                            buffer[y][x] = TileType.Plain;
                        }
                    }
                }
            }

            // Mountain
            float targetMountainPercentage = aParemeters.Data.mountainPercentage;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if (buffer[y][x] != TileType.Water && randomizer.Next(0, 101) < 40 + 40.0f * targetMountainPercentage)
                    {
                        buffer[y][x] = TileType.Mountain;
                    }
                }
            }

            for (int i = 0; i < 5; ++i)
            {
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        if ((buffer[y][x] == TileType.Plain || buffer[y][x] == TileType.Forest) && (GetAliveNeighbourCount(TileType.Forest, buffer, y, x) >= 5 || GetAliveNeighbourCount(TileType.Plain, buffer, y, x) >= 5))
                        {
                            buffer[y][x] = TileType.Mountain;
                        }
                        if ((buffer[y][x] == TileType.Mountain) && (GetAliveNeighbourCount(TileType.Forest, buffer, y, x) < 4 || GetAliveNeighbourCount(TileType.Plain, buffer, y, x) < 4))
                        {
                            buffer[y][x] = TileType.Forest;
                        }
                    }
                }
            }
        }

        private TileType GetRandomLandType()
        {
            TileType rv = new TileType();
            rv = (TileType)randomizer.Next(1, 4);
            return rv;
        }

        private void InsertIsland(int x, int y, int size)
        {
            myWorldData[y][x] = TileType.Plain;
        }

        private int GetAliveNeighbourCount(TileType aliveCondition, List<List<TileType>> buffer, int y, int x)
        {
            int rv = 0;
            int width = myWorldData[0].Count;
            int height = myWorldData.Count;

            if (y > 0)
            {
                if (buffer[y - 1][x] == aliveCondition)
                    rv++;
            }
            if (y < height - 1)
            {
                if (buffer[y + 1][x] == aliveCondition)
                    rv++;
            }
            if (x > 0)
            {
                if (buffer[y][x-1] == aliveCondition)
                    rv++;
            }
            if (x < width - 1)
            {
                if (buffer[y][x + 1] == aliveCondition)
                    rv++;
            }
             
            // Diagonal
            if (y > 0 && x > 0)
            {
                if (buffer[y - 1][x - 1] == aliveCondition)
                    rv++;
            }
            if (y < height - 1 && x < width - 1)
            {
                if (buffer[y + 1][x + 1] == aliveCondition)
                    rv++;
            }
            if (x > 0 && y < height - 1)
            {
                if (buffer[y + 1][x - 1] == aliveCondition)
                    rv++;
            }
            if (x < width - 1 && y > 0)
            {
                if (buffer[y - 1][x + 1] == aliveCondition)
                    rv++;
            }

            return rv;
        }
    }
}
