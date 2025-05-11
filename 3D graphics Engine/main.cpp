#include <SFML/Graphics.hpp>
#include "Library.h" // Assuming this contains vec3, Ray, Object, Sphere, Triangle, HitInfo, random, randomDirection, normalize, dot etc.
#include <vector>
#include <fstream>
#include <string>    // Required for std::string and std::to_string
#include <cmath>     // For pow, std::cos, std::sin (std::cos, std::sin not needed for linear motion)
#include <algorithm> // For std::min, std::max (optional, for clamping)

#define WINDOW_WIDTH 250
#define WINDOW_HEIGHT 250

const int numSamples = 1000; // As in your provided code

vec3 eye(0.f, 0.f, 5.f);   // Non-const, will be updated for each meta-frame

const int maxDepth = 10;
const float PI_FLOAT = 3.1415926535f; // Define PI for calculations (not strictly needed for this camera motion)
const float brightness = (1000.f * PI_FLOAT) * (1.f / float(numSamples));

vec3 pathTrace(Ray ray, const std::vector<Object*>& objects, int depth) {
    if (depth > maxDepth) return vec3(0.f);
    double minT = 1e8;
    HitInfo hi, finalHi;
    for (auto obj : objects) {
        hi = obj->hit(ray);
        if (hi.hit && hi.t < minT) {
            minT = hi.t;
            finalHi = hi;
        }
    }
    if (!finalHi.hit) return vec3(0.f);
    if (finalHi.emissive) return finalHi.colour;
    float p = 0.8f;
    if (random() > p) return vec3(0.f);
    Ray r;
    r.origin = finalHi.hitLocation;
    r.direction = randomDirection(finalHi.normal);
    float cosine = fabs(dot(-ray.direction, finalHi.normal));
    vec3 newCol = pathTrace(r, objects, depth + 1) * cosine;
    return (newCol * finalHi.colour) / p;
}

int main() {
    std::vector<Object*> objects;

    // Scene setup (identical to your original code)
    Sphere sphere;
    sphere.centre = vec3(0.f, -0.7f, -0.5f);
    sphere.radius = 0.3f;
    sphere.colour = vec3(2.f);
    objects.push_back(&sphere);

    Sphere sphere2;
    sphere2.centre = vec3(0.8f, -0.7f, -0.5f);
    sphere2.radius = 0.3f;
    sphere2.colour = vec3(1.f, 0.f, 0.f);
    objects.push_back(&sphere2);

    Triangle floor1;
    floor1.v1 = vec3(-50.f, -1.f, 50.f);
    floor1.v2 = vec3(50.f, -1.f, 50.f);
    floor1.v3 = vec3(50.f, -1.f, -50.f);
    floor1.colour = vec3(0.8f, 0.8f, 0.8f);
    objects.push_back(&floor1);

    Triangle floor2;
    floor2.v1 = vec3(-50.f, -1.f, 50.f);
    floor2.v2 = vec3(50.f, -1.f, -50.f);
    floor2.v3 = vec3(-50.f, -1.f, -50.f);
    floor2.colour = vec3(0.8f, 0.8f, 0.8f);
    objects.push_back(&floor2);

    Triangle light1;
    light1.v1 = vec3(0.5f, 0.99f, 0.5f);
    light1.v2 = vec3(-0.5f, 0.99f, -0.5f);
    light1.v3 = vec3(-0.5f, 0.99f, 0.5f);
    light1.colour = vec3(2.f);
    light1.emissive = true;
    objects.push_back(&light1);

    Triangle light2;
    light2.v1 = vec3(0.5f, 0.99f, 0.5f);
    light2.v2 = vec3(0.5f, 0.99f, -0.5f);
    light2.v3 = vec3(-0.5f, 0.99f, -0.5f);
    light2.colour = vec3(2.f);
    light2.emissive = true;
    objects.push_back(&light2);

    auto addPyramid = [&](vec3 P, float size, vec3 col) {
        vec3 b0 = P + vec3(-size, 0.f, -size);
        vec3 b1 = P + vec3(size, 0.f, -size);
        vec3 b2 = P + vec3(size, 0.f, size);
        vec3 b3 = P + vec3(-size, 0.f, size);
        vec3 tip = P + vec3(0.f, size * 1.5f, 0.f);
        static Triangle tbase1, tbase2;
        tbase1.v1 = b0; tbase1.v2 = b1; tbase1.v3 = b2; tbase1.colour = col; objects.push_back(&tbase1);
        tbase2.v1 = b0; tbase2.v2 = b2; tbase2.v3 = b3; tbase2.colour = col; objects.push_back(&tbase2);
        static Triangle s0, s1, s2, s3;
        s0.v1 = b0; s0.v2 = b1; s0.v3 = tip; s0.colour = col; objects.push_back(&s0);
        s1.v1 = b1; s1.v2 = b2; s1.v3 = tip; s1.colour = col; objects.push_back(&s1);
        s2.v1 = b2; s2.v2 = b3; s2.v3 = tip; s2.colour = col; objects.push_back(&s2);
        s3.v1 = b3; s3.v2 = b0; s3.v3 = tip; s3.colour = col; objects.push_back(&s3);
        };
    addPyramid(vec3(-0.8f, -1.f, -0.5f), 0.5f, vec3(0.2f, 0.8f, 0.2f));

    double* image = new double[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT * 3; ++i) {
        image[i] = 0.0;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray Tracing");
    sf::Image sfImage;
    sfImage.create(WINDOW_WIDTH, WINDOW_HEIGHT, sf::Color::Black);
    sf::Texture texture;
    texture.loadFromImage(sfImage);
    sf::Sprite sprite(texture);

    float samples_count_float = 0;
    int current_meta_frame_number = 1;
    const int total_meta_frames = 5;

    // Camera animation parameters for moving towards shapes
    const vec3 start_eye_position(0.f, 0.5f, 5.0f); // Start further away, slightly elevated
    // End closer to the scene. Image plane is at z=1.2. Keep eye.z > 1.2 for this ray setup.
    const vec3 end_eye_position(0.f, 0.0f, 1.5f);

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (current_meta_frame_number <= total_meta_frames) {

            if (samples_count_float == 0) {
                // Calculate and update the global 'eye' position for the current meta-frame
                // Linearly interpolate 'eye' from start_eye_position to end_eye_position
                float t = 0.0f;
                if (total_meta_frames > 1) {
                    // t goes from 0 for the first frame to 1 for the last frame
                    t = (float)(current_meta_frame_number - 1) / (float)(total_meta_frames - 1);
                }
                else {
                    // If only 1 meta frame, place it at the end position (or start, it's a choice)
                    t = 1.0f;
                }

                eye.x = start_eye_position.x * (1.0f - t) + end_eye_position.x * t;
                eye.y = start_eye_position.y * (1.0f - t) + end_eye_position.y * t;
                eye.z = start_eye_position.z * (1.0f - t) + end_eye_position.z * t;
            }

            if (samples_count_float < numSamples) {
                double* p_accum = image;
                for (int y_pixel = 0; y_pixel < WINDOW_HEIGHT; ++y_pixel) {
                    for (int x_pixel = 0; x_pixel < WINDOW_WIDTH; ++x_pixel) {
                        float i = (2.f * (x_pixel + 0.5f) / WINDOW_WIDTH) - 1.f;
                        float j = (2.f * (WINDOW_HEIGHT - y_pixel + 0.5f) / WINDOW_HEIGHT) - 1.f;

                        vec3 pixel_on_image_plane(i, j, 1.2f);
                        vec3 dir = normalize(pixel_on_image_plane - eye);

                        Ray ray;
                        ray.origin = pixel_on_image_plane;
                        ray.direction = dir;

                        vec3 col = pathTrace(ray, objects, 0) * brightness;
                        *p_accum++ += col.r;
                        *p_accum++ += col.g;
                        *p_accum++ += col.b;
                    }
                }

                double* p_read = image;
                float display_divisor = samples_count_float + 1.0f;
                for (int y_pixel = 0; y_pixel < WINDOW_HEIGHT; ++y_pixel) {
                    for (int x_pixel = 0; x_pixel < WINDOW_WIDTH; ++x_pixel) {
                        double r_sum = *p_read++;
                        double g_sum = *p_read++;
                        double b_sum = *p_read++;
                        int r_val = int(pow(r_sum / display_divisor, 1 / 2.2f) * 255);
                        int g_val = int(pow(g_sum / display_divisor, 1 / 2.2f) * 255);
                        int b_val = int(pow(b_sum / display_divisor, 1 / 2.2f) * 255);
                        sfImage.setPixel(x_pixel, y_pixel, sf::Color(static_cast<sf::Uint8>(r_val),
                            static_cast<sf::Uint8>(g_val),
                            static_cast<sf::Uint8>(b_val)));
                    }
                }
                texture.update(sfImage);
                samples_count_float++;
            }

            if (static_cast<int>(samples_count_float) == numSamples) {
                std::string ppm_filename = "render" + std::to_string(current_meta_frame_number) + ".ppm";
                std::ofstream ppm(ppm_filename);
                ppm << "P3\n" << WINDOW_WIDTH << " " << WINDOW_HEIGHT << "\n255\n";
                double* q = image;
                for (int yy = 0; yy < WINDOW_HEIGHT; ++yy) {
                    for (int xx = 0; xx < WINDOW_WIDTH; ++xx) {
                        double r_sum = *q++;
                        double g_sum = *q++;
                        double b_sum = *q++;
                        ppm << int(pow(r_sum / numSamples, 1 / 2.2f) * 255) << " "
                            << int(pow(g_sum / numSamples, 1 / 2.2f) * 255) << " "
                            << int(pow(b_sum / numSamples, 1 / 2.2f) * 255) << "\n";
                    }
                }
                ppm.close();

                current_meta_frame_number++;

                if (current_meta_frame_number <= total_meta_frames) {
                    samples_count_float = 0;
                    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT * 3; ++i) {
                        image[i] = 0.0;
                    }
                }
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    delete[] image;
    return 0;
}