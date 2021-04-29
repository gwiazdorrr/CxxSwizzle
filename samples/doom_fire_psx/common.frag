#ifndef CXX_CONST
#define CXX_CONST const
#define CXX_IGNORE(x) x
#define CXX_ARRAY_AUTO(type) type[]
#define CXX_ARRAY(type) type[]
#define CXX_ARRAY_N(type, size) type[size]
#define CXX_MAKE_ARRAY(type) type[]
#endif

#define RGB(x) vec4(float((x >> 16) & 0xFF) / 255.0, float((x >> 8) & 0xFF) / 255.0, float(x & 0xFF) / 255.0, 1.0)
CXX_ARRAY_FIELD(vec4, colors)(
    RGB(0x070707),
    RGB(0x1f0707),
    RGB(0x2f0f07),
    RGB(0x470f07),
    RGB(0x571707),
    RGB(0x671f07),
    RGB(0x771f07),
    RGB(0x8f2707),
    RGB(0x9f2f07),
    RGB(0xaf3f07),
    RGB(0xbf4707),
    RGB(0xc74707),
    RGB(0xDF4F07),
    RGB(0xDF5707),
    RGB(0xDF5707),
    RGB(0xD75F07),
    RGB(0xD7670F),
    RGB(0xcf6f0f),
    RGB(0xcf770f),
    RGB(0xcf7f0f),
    RGB(0xCF8717),
    RGB(0xC78717),
    RGB(0xC78F17),
    RGB(0xC7971F),
    RGB(0xBF9F1F),
    RGB(0xBF9F1F),
    RGB(0xBFA727),
    RGB(0xBFA727),
    RGB(0xBFAF2F),
    RGB(0xB7AF2F),
    RGB(0xB7B72F),
    RGB(0xB7B737),
    RGB(0xCFCF6F),
    RGB(0xDFDF9F),
    RGB(0xEFEFC7),
    RGB(0xFFFFFF)
);