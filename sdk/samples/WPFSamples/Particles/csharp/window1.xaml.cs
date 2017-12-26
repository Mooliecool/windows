// #define USE_VISUALBRUSH

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Media.Media3D;
using System.Collections.Generic;
using System.Windows.Media.Imaging;

namespace Particles
{
    public class ParticleSystemManager
    {
        private Dictionary<System.Windows.Media.Color, ParticleSystem> particleSystems;

        public ParticleSystemManager()
        {
            this.particleSystems = new Dictionary<System.Windows.Media.Color, ParticleSystem>();
        }

        public void Update(float elapsed)
        {
            foreach (ParticleSystem ps in this.particleSystems.Values)
            {
                ps.Update(elapsed);
            }
        }

        public void SpawnParticle(Point3D position, double speed, System.Windows.Media.Color color, double size, double life)
        {
            try
            {
                ParticleSystem ps = this.particleSystems[color];
                ps.SpawnParticle(position, speed, size, life);
            }
            catch { }
        }

        public Model3D CreateParticleSystem(int maxCount, System.Windows.Media.Color color)
        {
            ParticleSystem ps = new ParticleSystem(maxCount, color);
            this.particleSystems.Add(color, ps);
            return ps.ParticleModel;
        }

        public int ActiveParticleCount
        {
            get
            {
                int count = 0;
                foreach (ParticleSystem ps in this.particleSystems.Values)
                    count += ps.Count;
                return count;
            }
        }
    }

    public class ParticleSystem
    {        
        private List<Particle> particleList;
        private GeometryModel3D particleModel;
        private int maxParticleCount;
        private Random rand;

        public ParticleSystem(int maxCount, System.Windows.Media.Color color)
        {
            this.maxParticleCount = maxCount;

            this.particleList = new List<Particle>();

            this.particleModel = new GeometryModel3D();
            this.particleModel.Geometry = new MeshGeometry3D();

            Ellipse e = new Ellipse();
            e.Width = 32.0;
            e.Height = 32.0;
            RadialGradientBrush b = new RadialGradientBrush();
            b.GradientStops.Add(new GradientStop(System.Windows.Media.Color.FromArgb(0xFF, color.R, color.G, color.B), 0.25));
            b.GradientStops.Add(new GradientStop(System.Windows.Media.Color.FromArgb(0x00, color.R, color.G, color.B), 1.0));
            e.Fill = b;
            e.Measure(new System.Windows.Size(32, 32));
            e.Arrange(new Rect(0, 0, 32, 32));

            System.Windows.Media.Brush brush = null;

#if USE_VISUALBRUSH
            brush = new VisualBrush(e);
#else
            RenderTargetBitmap renderTarget = new RenderTargetBitmap(32, 32, 96, 96, PixelFormats.Pbgra32);
            renderTarget.Render(e);
            renderTarget.Freeze();
            brush = new ImageBrush(renderTarget);
#endif

            DiffuseMaterial material = new DiffuseMaterial(brush);

            this.particleModel.Material = material;

            this.rand = new Random(brush.GetHashCode());
        }

        public void Update(double elapsed)
        {
            List<Particle> deadList = new List<Particle>();

            // Update all particles
            foreach (Particle p in this.particleList)
            {
                p.Position += p.Velocity * elapsed;
                p.Life -= p.Decay * elapsed;
                p.Size = p.StartSize * (p.Life / p.StartLife);
                if (p.Life <= 0.0)
                    deadList.Add(p);
            }

            foreach (Particle p in deadList)
                this.particleList.Remove(p);

            UpdateGeometry();
        }

        private void UpdateGeometry()
        {
            Point3DCollection positions = new Point3DCollection();
            Int32Collection indices = new Int32Collection();
            PointCollection texcoords = new PointCollection();

            for (int i = 0; i < this.particleList.Count; ++i)
            {
                int positionIndex = i * 4;
                int indexIndex = i * 6;
                Particle p = this.particleList[i];

                Point3D p1 = new Point3D(p.Position.X, p.Position.Y, p.Position.Z);
                Point3D p2 = new Point3D(p.Position.X, p.Position.Y + p.Size, p.Position.Z);
                Point3D p3 = new Point3D(p.Position.X + p.Size, p.Position.Y + p.Size, p.Position.Z);
                Point3D p4 = new Point3D(p.Position.X + p.Size, p.Position.Y, p.Position.Z);

                positions.Add(p1);
                positions.Add(p2);
                positions.Add(p3);
                positions.Add(p4);

                System.Windows.Point t1 = new System.Windows.Point(0.0, 0.0);
                System.Windows.Point t2 = new System.Windows.Point(0.0, 1.0);
                System.Windows.Point t3 = new System.Windows.Point(1.0, 1.0);
                System.Windows.Point t4 = new System.Windows.Point(1.0, 0.0);

                texcoords.Add(t1);
                texcoords.Add(t2);
                texcoords.Add(t3);
                texcoords.Add(t4);

                indices.Add(positionIndex);
                indices.Add(positionIndex+2);
                indices.Add(positionIndex+1);
                indices.Add(positionIndex);
                indices.Add(positionIndex+3);
                indices.Add(positionIndex+2);
            }

            ((MeshGeometry3D)this.particleModel.Geometry).Positions = positions;
            ((MeshGeometry3D)this.particleModel.Geometry).TriangleIndices = indices;
            ((MeshGeometry3D)this.particleModel.Geometry).TextureCoordinates = texcoords;

        }

        public void SpawnParticle(Point3D position, double speed, double size, double life)
        {
            if (this.particleList.Count > this.maxParticleCount)
                return;
            Particle p = new Particle();
            p.Position = position;
            p.StartLife = life;
            p.Life = life;
            p.StartSize = size;
            p.Size = size;

            float x = 1.0f - (float)rand.NextDouble() * 2.0f;
            float z = 1.0f - (float)rand.NextDouble() * 2.0f;

            Vector3D v = new Vector3D(x, z, 0.0);
            v.Normalize();
            v *= ((float)rand.NextDouble() + 0.25f) * (float)speed;

            p.Velocity = new Vector3D(v.X, v.Y, v.Z);

            p.Decay = 1.0f;// 0.5 + rand.NextDouble();
            //if (p.Decay > 1.0)
            //    p.Decay = 1.0;

            this.particleList.Add(p);
        }

        public int MaxParticleCount
        {
            get
            {
                return this.maxParticleCount;
            }
            set
            {
                this.maxParticleCount = value;
            }
        }

        public int Count
        {
            get
            {
                return this.particleList.Count;
            }
        }

        public Model3D ParticleModel
        {
            get
            {
                return this.particleModel;
            }
        }
    }


    public class Particle
    {
        public Point3D Position;
        public Vector3D Velocity;
        public double StartLife;
        public double Life;
        public double Decay;
        public double StartSize;
        public double Size;
    }

    public partial class Window1 : Window
    {
        System.Windows.Threading.DispatcherTimer frameTimer;

        private Point3D spawnPoint;
        private double elapsed;
        private double totalElapsed;
        private int lastTick;
        private int currentTick;

        private int frameCount;
        private double frameCountTime;
        private int frameRate;

        private ParticleSystemManager pm;

        private Random rand;

        public Window1()
        {
            InitializeComponent();

            frameTimer = new System.Windows.Threading.DispatcherTimer();
            frameTimer.Tick += OnFrame;
            frameTimer.Interval = TimeSpan.FromSeconds(1.0 / 60.0);
            frameTimer.Start();

            this.spawnPoint = new Point3D(0.0, 0.0, 0.0);
            this.lastTick = Environment.TickCount;

            pm = new ParticleSystemManager();

            this.WorldModels.Children.Add(pm.CreateParticleSystem(1000, Colors.Gray));
            this.WorldModels.Children.Add(pm.CreateParticleSystem(1000, Colors.Red));
            this.WorldModels.Children.Add(pm.CreateParticleSystem(1000, Colors.Silver));
            this.WorldModels.Children.Add(pm.CreateParticleSystem(1000, Colors.Orange));
            this.WorldModels.Children.Add(pm.CreateParticleSystem(1000, Colors.Yellow));

            rand = new Random(this.GetHashCode());

            this.KeyDown += new System.Windows.Input.KeyEventHandler(Window1_KeyDown);
            this.Cursor = System.Windows.Input.Cursors.None;
        }

        void Window1_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Escape)
                this.Close();
        }

        private void OnFrame(object sender, EventArgs e)
        {
            // Calculate frame time;
            this.currentTick = Environment.TickCount;
            this.elapsed = (double)(this.currentTick - this.lastTick) / 1000.0;
            this.totalElapsed += this.elapsed;
            this.lastTick = this.currentTick;

            frameCount++;
            frameCountTime += elapsed;
            if (frameCountTime >= 1.0)
            {
                frameCountTime -= 1.0;
                frameRate = frameCount;
                frameCount = 0;
                this.FrameRateLabel.Content = "FPS: " + frameRate.ToString() + "  Particles: " + pm.ActiveParticleCount.ToString();
            }

            pm.Update((float)elapsed);
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Red, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Orange, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Silver, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Gray, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Red, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Orange, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Silver, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Gray, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Red, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Yellow, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Silver, rand.NextDouble(), 2.5 * rand.NextDouble());
            pm.SpawnParticle(this.spawnPoint, 10.0, Colors.Yellow, rand.NextDouble(), 2.5 * rand.NextDouble());

            double c = Math.Cos(this.totalElapsed);
            double s = Math.Sin(this.totalElapsed);

            this.spawnPoint = new Point3D(s * 32.0, c * 32.0, 0.0);
        }

    }
}