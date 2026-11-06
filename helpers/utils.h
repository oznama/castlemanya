typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t);

void load_level(uint8_t currentLevel);