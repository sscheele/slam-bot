using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Windows;
using HtmlAgilityPack;
using System.Text.RegularExpressions;


namespace LidarVisualizer
{
    public partial class Form1 : Form
    {

        Graphics graphics;
        int edge_x;
        int edge_y;
        int center;
        int mapping_size;
        int data_size;

        PointF corner;
        PointF map_center;
        Size map_size;

        LinkedList<PointF> obstacles;
        string data;
        string file = Path.GetFullPath("C:/Users/semcd/Desktop/ECEFinal/LIDARVisualizer/LidarVisualizer/data.txt");
        Dictionary<(int, int), (float, float)> addition = new Dictionary<(int, int), (float, float)>
        {
            {(0,0),((float) 0.0, (float) 0.0)}, {(0,1),((float) 0.0, (float) -22.86)}, {(0,2),((float) 0.0,(float) 45.72)},
            {(1,0),((float) 15.24, (float) 0.0)}, {(1,1),((float) 15.24, (float) -22.86)}, {(1,2),((float) 15.24,(float) 45.72)},
            {(2,0),((float) 30.48,(float) 0.0)}, {(2,1),((float) 30.48,(float) -22.86)}, {(2,2),((float) 30.48, (float) 45.72)},
            {(3,0),((float) 45.72, (float) 0.0)}, {(3,1),((float) 45.72, (float) -22.86)}, {(3,2),((float) 45.72, (float) 45.72)}
        };

        public Form1()
        {
            InitializeComponent();
            this.Width = 500;
            this.Height = 500;
            this.graphics = this.CreateGraphics();
            this.obstacles = new LinkedList<PointF>();
            this.data = "";
            this.edge_x = 40;
            this.edge_y = 20;
            this.center = 10;
            this.mapping_size = 350;
            this.data_size = 2;
            this.corner = new PointF(edge_x, edge_y);
            this.map_center = new PointF(corner.X + mapping_size / 2, corner.Y + mapping_size / 2);
            this.map_size = map_size = new Size(mapping_size, mapping_size);
        }
        
        private void receiveData()
        {
            // IP address of the pi used
            var url = "http://172.16.107.137";
            var web = new HtmlWeb();
            var doc = web.Load(url);
            //HtmlNode[] body = doc.DocumentNode.SelectNodes("//body");
            //string bodyContent = body.InnerText;
            this.data = doc.DocumentNode.SelectSingleNode("//body").InnerText;

            

            this.obstacles = new LinkedList<PointF>();
            try
            {

                int col = 0;
                int row = 0;
                int nums = 0;
                int count = 0;

                string[] lines = data.Split("\n");

                foreach (string line in lines)
                {

                    
                    if (count > 0)
                    {
                        string[] split = line.Split(',');
                        split[1] = Regex.Replace(split[1], "[^0-9.]", "");
                        //string string1 = split[0];
                        //string string2 = split[1];
                        float x = float.Parse(split[0]);
                        float y = float.Parse(split[1]);

                        (float addx, float addy) = addition[(row, col)];

                        obstacles.AddLast(new PointF(x + addx, y + addy));
                        

                       if (nums < 7)
                        {
                            nums++;
                        }
                        else
                        {
                            row++;
                            if (row == 2)
                            {
                                col++;
                            }
                            nums = 0;
                        }
                       
                    }
                    count++;
                    }
            }
            catch
            {
                throw new Exception("Data could not be parsed");
            }
        }

        private Rectangle centerPoint(PointF center, Size size)
        {
            return new Rectangle((int)center.X - size.Width / 2, (int)center.Y - size.Height / 2, size.Width, size.Height);
        }

        private void generateMap(Graphics graphics, PointF center, Size size)
        {
            foreach(PointF p in obstacles)
            {
                float x = p.X;
                float y = p.Y;
                PointF wall = new PointF(center.X + x, center.Y + y);
                graphics.FillRectangle(Brushes.Black, centerPoint(wall, new Size(this.data_size, this.data_size)));
            }
        }
        
        private void form1_Draw(object sender, PaintEventArgs e)
        {
            graphics.DrawRectangle(Pens.Black, centerPoint(this.map_center, this.map_size));
            graphics.FillRectangle(Brushes.Black, centerPoint(this.map_center, new Size(center, this.center)));
        }

        private void form1_Load(object sender, EventArgs e)
        {

        }
        private void clearVisualizer(Graphics graphics, PointF center, Size size)
        {
            graphics.FillRectangle(new SolidBrush(this.BackColor), centerPoint(center, size));
            graphics.DrawRectangle(Pens.Black, centerPoint(this.map_center, this.map_size));
            graphics.FillRectangle(Brushes.Black, centerPoint(this.map_center, new Size(this.center, this.center)));
        }

        private void clearButtonClick(object sender, EventArgs e)
        {
            clearVisualizer(graphics, this.map_center, this.map_size);
        }

        private void receiveButtonClick(object sender, EventArgs e)
        {
            receiveData();
            generateMap(graphics, this.map_center,this.map_size);
        }

    }
}
