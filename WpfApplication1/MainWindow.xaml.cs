using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using Microsoft.Win32;
using Newtonsoft.Json;

namespace Tool
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        WorldParameters worldParameters;
        string directory;
        bool isManuallyControllingMountain = false, isManuallyControllingForest = false, isManuallyControllingPlain = false;
        public MainWindow()
        {
            worldParameters = new WorldParameters();

            InitializeComponent();

            directory = Directory.GetCurrentDirectory();
            Directory.CreateDirectory(directory + "/Worlds");

            textBox_Width.Text = worldParameters.Data.width.ToString();
            textBox_Height.Text = worldParameters.Data.height.ToString();
            slider_Water.Value = worldParameters.Data.waterPercentage;
            slider_WaterBodyBias.Value = worldParameters.Data.waterBodyBias;
            slider_Land.Value = worldParameters.Data.landPercentage;
            slider_Plain.Value = worldParameters.Data.plainPercentage;
            slider_Forest.Value = worldParameters.Data.forestPercentage;
            slider_Mountain.Value = worldParameters.Data.mountainPercentage;
        }

        private void button_Load_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Title = "Load World";
            dialog.InitialDirectory = directory + "\\Worlds";

            dialog.ShowDialog();

            if (File.Exists(dialog.FileName))
            {
                worldParameters.LoadFromFile(dialog.FileName);
            }
        }

        private void button_Save_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.Title = "Save World";
            dialog.InitialDirectory = directory + "\\Worlds";
            dialog.DefaultExt = ".json";
            dialog.AddExtension = true;
            dialog.FileName = "world";

            dialog.ShowDialog();

            if (dialog.CheckPathExists)
            {
                worldParameters.SaveToFile(dialog.FileName);
            }
        }

        private void textBox_Width_TextChanged(object sender, TextChangedEventArgs e)
        {
            uint result;
            if (uint.TryParse(textBox_Width.Text, out result))
            {
                worldParameters.Data.width = result;
            }
        }

        private void textBox_Height_TextChanged(object sender, TextChangedEventArgs e)
        {
            uint result = 0;
            if (uint.TryParse(textBox_Width.Text, out result))
            {
                worldParameters.Data.height = result;
            }
        }

        private void slider_Water_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            slider_Land.Value = 1.0f - slider_Water.Value;
        }

        private void slider_Land_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            slider_Water.Value = 1.0f - slider_Land.Value;
        }

        private void button_Generate_Click(object sender, RoutedEventArgs e)
        {
            uint result;
            if (uint.TryParse(textBox_Width.Text, out result))
            {
                worldParameters.Data.width = result;
            }
            if (uint.TryParse(textBox_Height.Text, out result))
            {
                worldParameters.Data.height = result;
            }
            worldParameters.Data.waterPercentage = (float)slider_Water.Value;
            worldParameters.Data.waterBodyBias = (float)slider_WaterBodyBias.Value;
            worldParameters.Data.landPercentage = (float)slider_Land.Value;
            worldParameters.Data.plainPercentage = (float)slider_Plain.Value;
            worldParameters.Data.forestPercentage = (float)slider_Forest.Value;
            worldParameters.Data.mountainPercentage  = (float)slider_Mountain.Value;

            World world = new World();
            world.Generate(worldParameters);

            string fileData = JsonConvert.SerializeObject(world, Formatting.Indented);

            StreamWriter sw = new StreamWriter(directory + "\\world.json");
            sw.Write(fileData);

            sw.Close();
        }

        private void slider_Plain_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isManuallyControllingMountain && !isManuallyControllingForest)
            {
                isManuallyControllingPlain = true;
                float mountain = (float)slider_Mountain.Value;
                float forest = (float)slider_Forest.Value;

                float total = mountain + forest;
                if (total == 0)
                {
                    total = 1.0f;
                    mountain = 0.5f;
                    forest = 0.5f;
                }

                slider_Mountain.Value = (1.0f - slider_Plain.Value) * (mountain / total);
                slider_Forest.Value = (1.0f - slider_Plain.Value) * (forest / total);
                isManuallyControllingPlain = false;
            }
        }

        private void slider_Forest_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isManuallyControllingPlain && !isManuallyControllingMountain)
            {
                isManuallyControllingForest = true;
                float mountain = (float)slider_Mountain.Value;
                float plain = (float)slider_Plain.Value;

                float total = mountain + plain;
                if (total == 0)
                {
                    total = 1.0f;
                    mountain = 0.5f;
                    plain = 0.5f;
                }

                slider_Plain.Value = (1.0f - slider_Forest.Value) * (plain / total);
                slider_Mountain.Value = (1.0f - slider_Forest.Value) * (mountain / total);
                isManuallyControllingForest = false;
            }
        }

        private void slider_Mountain_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isManuallyControllingForest && !isManuallyControllingPlain)
            {
                isManuallyControllingMountain = true;
                float plain = (float)slider_Plain.Value;
                float forest = (float)slider_Forest.Value;

                float total = plain + forest;
                if (total == 0)
                {
                    total = 1.0f;
                    plain = 0.5f;
                    forest = 0.5f;
                }

                slider_Plain.Value = (1.0f - slider_Mountain.Value) * (plain / total);
                slider_Forest.Value = (1.0f - slider_Mountain.Value) * (forest / total);
                isManuallyControllingMountain = false;
            }
        }
    }
}
