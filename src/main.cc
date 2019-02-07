#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <complex>
#include <cmath>

#include <SFML/Graphics.hpp>

#define MAX_ITERATION 255
#define EPSILON       0.00001
#define SIZE_PALETTE  1792

sf::Vector2<double> g_origin;
double g_echelon;
sf::Color g_palette[SIZE_PALETTE];

void
initPalette()
{
  for (unsigned int i = 0; i < 256; i++)
  {
    g_palette[i] = sf::Color(i, 0, i);                // black to magenta
    g_palette[i + 256] = sf::Color(255 - i, 0, 255);  // magenta to blue
    g_palette[i + 512] = sf::Color(0, i, 255);        // blue to cyan
    g_palette[i + 768] = sf::Color(0, 255, 255 - i);  // cyan to green
    g_palette[i + 1014] = sf::Color(i, 255, 0);       // green to yellow
    g_palette[i + 1280] = sf::Color(255, 255 - i, 0); // yellow to red
    g_palette[i + 1536] = sf::Color(255 - i, 0, 0);   // red to black
  }
}

inline void
drawPoint(std::complex<double> z, sf::Image* image, sf::Color color)
{
  // change of coordinates ((0, 0), x, y) -> ((x0, y0), x, -y)
  double x = z.real() * g_echelon + g_origin.x;
  double y = -z.imag() * g_echelon + g_origin.y;

  image->setPixel(x, y, color);
}

inline std::complex<double>
getPoint(int x, int y)
{
  return std::complex<double>((x - g_origin.x) * 1.0 / g_echelon,
                              (y - g_origin.y) * 1.0 / -g_echelon);
}

inline void
moveOrigin(sf::Image* image, std::complex<double> z)
{
  if (z == 0.0)
  {
    g_origin.x = (2.0 / 3.0) * image->getSize().x;
    g_origin.y = (1.0 / 2.0) * image->getSize().y;
    return;
  }

  // change of coordinates ((0, 0), x, y) -> ((x0, y0), x, -y)
  double x = z.real() * g_echelon + g_origin.x;
  double y = -z.imag() * g_echelon + g_origin.y;

  g_origin.x += image->getSize().x / 2.0 - x;
  g_origin.y += image->getSize().x / 2.0 - y;
}

inline sf::Color
isInMandelbrotSet(std::complex<double> c)
{
  std::complex<double> z_p = 0.0; // previous term
  std::complex<double> z_n;       // next term

  for (unsigned int j = 0; j < MAX_ITERATION; ++j)
  {
    z_n = (z_p * z_p) + c;
    z_p = z_n;

    if (std::abs(z_n) >= 2)
      //return sf::Color(0, j, 0);
      return g_palette[j * (SIZE_PALETTE - 1) / MAX_ITERATION];
  }

  return sf::Color::Black;
}

void
drawMandelbrotSet(sf::Image* image)
{
  std::complex<double> z_start = getPoint(0, 0);
  std::complex<double> z = getPoint(0, 0);
  std::complex<double> z_end = getPoint(image->getSize().x - 2,
                                        image->getSize().x - 2);


  while (std::abs(z - z_end) >= EPSILON) // i.e. while (z != z_end)
  {
    // std::cout << z_start << " | " << z << " | " << z_end << std::endl;
    drawPoint(z, image, isInMandelbrotSet(z));

    if (std::abs(z.real() - z_end.real()) < EPSILON)
    {
      z.real(z_start.real());
      z.imag(z.imag() - 1.0 / g_echelon);
    }
    else
      z += 1.0 / g_echelon;
  }
}

int main(int argc, char* argv[])
{
  if (argc != 3
      || (std::strcmp(argv[1], "-w") && std::strcmp(argv[1], "--width")))
  {
    std::cerr << "error: usage: " << argv[0] << " -w|--width <INTEGER>\n";
    std::exit(1);
  }
  int width = std::atoi(argv[2]);

  sf::RenderWindow window(sf::VideoMode(width, width), "My Fractal");
  window.setVerticalSyncEnabled(true); // enable vertical synchronization

  sf::Image image;
  image.create(width, width, sf::Color::Black);

  g_origin.x = (2.0 / 3.0) * image.getSize().x;
  g_origin.y = (1.0 / 2.0) * image.getSize().y;

  g_echelon = (1.0 / 3.0) * image.getSize().x;

  initPalette();
  drawMandelbrotSet(&image);

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type)
      {
        case sf::Event::Closed:
        case sf::Event::MouseButtonReleased:
        //case sf::Event::KeyReleased:
          window.close();
          break;

        case sf::Event::MouseWheelScrolled:
          std::cout << event.mouseWheelScroll.delta;
          std::cout << std::flush;
          if (event.mouseWheelScroll.delta == 1)
          {
            g_echelon += width / 3.0;
            moveOrigin(&image, getPoint(event.mouseWheelScroll.x,
                                        event.mouseWheelScroll.y));
          }
          else
          {
            if ((g_echelon - width / 3.0) > 0)
            {
              g_echelon -= width / 3.0;
              moveOrigin(&image, getPoint(event.mouseWheelScroll.x,
                                          event.mouseWheelScroll.y));
            }
            else
              moveOrigin(&image, std::complex<double>(0, 0));
          }
          drawMandelbrotSet(&image);
          std::cout << " | " << g_echelon << std::endl;
          std::cout << "DONE --------------------------" << std::endl;
          break;

        default:
          break;
      }
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    texture.setSmooth(true);

    sf::Sprite sprite(texture);

    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
