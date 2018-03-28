using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;

namespace Tool
{
    class WorldParameters
    {
        public struct SData
        {
            public uint width;
            public uint height;

            public float waterPercentage;
            public float waterBodyBias;

            public float landPercentage;
            public float plainPercentage;
            public float forestPercentage;
            public float mountainPercentage;
        }

        public SData Data;

        public WorldParameters()
        {
            Data.width = 64;
            Data.height = 64;
            Data.waterPercentage = 0.35f;
            Data.waterBodyBias = 0.5f;
            Data.landPercentage = 0.65f;
            Data.plainPercentage = 0.33333f;
            Data.forestPercentage = 0.33333f;
            Data.mountainPercentage = 0.33333f;
        }

        public void SaveToFile(string aFilePath)
        {
            string fileData = JsonConvert.SerializeObject(Data, Formatting.Indented);

            StreamWriter sw = new StreamWriter(aFilePath);
            sw.Write(fileData);

            sw.Close();
        }

        public void LoadFromFile(string aFilePath)
        {
            StreamReader sr = new StreamReader(aFilePath);

            string fileData = sr.ReadToEnd();

            Data = JsonConvert.DeserializeObject<SData>(fileData);
        }
    }

}
