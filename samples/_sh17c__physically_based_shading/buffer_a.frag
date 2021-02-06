// control loop

const float MENU_SURFACE = 0.;
const float MENU_METAL = 1.;
const float MENU_DIELECTRIC = 2.;
const float MENU_ROUGHNESS = 3.;
const float MENU_BASE_COLOR = 4.;
const float MENU_LIGHTING = 5.;
const float MENU_DIFFUSE = 6.;
const float MENU_SPECULAR = 7.;
const float MENU_DISTR = 8.;
const float MENU_FRESNEL = 9.;
const float MENU_GEOMETRY = 10.;

const float FOCUS_SLIDER = 1.;
const float FOCUS_OBJ = 2.;
const float FOCUS_COLOR = 3.;

struct AppState
{
    float menuId;
    float metal;
    float roughness;
    float baseColor;
    float focus;
    float focusObjRot;
    float objRot;
};

vec4 LoadValue(int x, int y)
{
    return texelFetch(iChannel0, ivec2(x, y), 0);
}

void LoadState(out AppState s)
{
    vec4 data;

    data = LoadValue(0, 0);
    s.menuId = data.x;
    s.metal = data.y;
    s.roughness = data.z;
    s.baseColor = data.w;

    data = LoadValue(1, 0);
    s.focus = data.x;
    s.focusObjRot = data.y;
    s.objRot = data.z;
}

void StoreValue(vec2 re, vec4 va, inout vec4 fragColor, vec2 fragCoord)
{
    fragCoord = floor(fragCoord);
    fragColor = (fragCoord.x == re.x && fragCoord.y == re.y) ? va : fragColor;
}

vec4 SaveState(in AppState s, in vec2 fragCoord)
{
    vec4 ret = vec4(0.);
    StoreValue(vec2(0., 0.), vec4(s.menuId, s.metal, s.roughness, s.baseColor), ret, fragCoord);
    StoreValue(vec2(1., 0.), vec4(s.focus, s.focusObjRot, s.objRot, 0.), ret, fragCoord);
    ret = iFrame >= 1 ? ret : vec4(0.);
    return ret;
}

float saturate(float x)
{
    return clamp(x, 0., 1.);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    if (fragCoord.x >= 8. || fragCoord.y >= 8.)
    {
        discard;
    }

    AppState s;
    LoadState(s);

    vec4 q = iMouse / iResolution.xyxy;
    vec4 m = -1. + 2. * q;
    m.xz *= iResolution.x / iResolution.y;
    m *= 100.;

    vec4 sliderM = m - vec2(-110, 74).xyxy;
    if (sliderM.z >= -4. && sliderM.z < 44. && sliderM.w >= -20. && sliderM.w < -10.)
    {
        s.focus = FOCUS_SLIDER;
    }
    else if (sliderM.z >= -4. && sliderM.z < 44. && sliderM.w >= -30. && sliderM.w < -20.)
    {
        s.focus = FOCUS_COLOR;
    }
    else if (sliderM.z >= -4. && sliderM.z < 6. && sliderM.w >= -10. && sliderM.w < -4.)
    {
        s.metal = 0.;
        s.menuId = s.menuId == MENU_METAL ? MENU_DIELECTRIC : s.menuId;
    }
    else if (sliderM.z >= -4. && sliderM.z < 6. && sliderM.w >= -4. && sliderM.w < 6.)
    {
        s.metal = 1.;
        s.menuId = s.menuId == MENU_DIELECTRIC ? MENU_METAL : s.menuId;
    }
    else if (m.w > -100. && m.w < 40. && abs(m.z + 20.) < 70.)
    {
        if (s.focus != FOCUS_OBJ)
        {
            s.focusObjRot = s.objRot;
        }
        s.focus = FOCUS_OBJ;
    }
    else
    {
        s.focus = 0.;
        vec2 mp = (m.xy - vec2(-160, -1));
        float menuId = mp.x < 40. || (mp.x < 60. && (mp.y > 18. && mp.y < 24.)) ? 10. - floor(mp.y / 8.) : -1.;
        if (menuId >= 0. && menuId <= 10.)
        {
            s.menuId = menuId;
        }
        s.metal = menuId == MENU_METAL ? 1. : s.metal;
        s.metal = menuId == MENU_DIELECTRIC ? 0. : s.metal;
    }

    if (s.focus == FOCUS_SLIDER)
    {
        s.roughness = saturate(sliderM.x / 40.);
    }
    if (s.focus == FOCUS_COLOR)
    {
        s.baseColor = floor(clamp((sliderM.x * 5.) / 32., 0., 5.));
    }
    if (s.focus == FOCUS_OBJ)
    {
        s.objRot = s.focusObjRot + .04 * (m.x - m.z);
    }

    fragColor = SaveState(s, fragCoord);
}