
main()
{
    float StencilValue;

    float Preview_Build;
    float Preview_Unbuild;
    float Friendly_Hover;
    float Friendly_Select;
    float Enemy_Hover;
    float Enemy_Select;

    float Preview_Build_Out_Color;
    float Preview_Unbuild_Out_Color;
    float Friendly_Hover_Out_Color;
    float Friendly_Select_Out_Color;
    float Enemy_Hover_Out_Color;
    float Enemy_Select_Out_Color;

    float DefaultSceneColor;

    if ((int)StencilValue == (int)Preview_Build)
        return Preview_Build_Out_Color;
    else if ((int)StencilValue == (int)Preview_Unbuild)
        return Preview_Unbuild_Out_Color;
    else if ((int)StencilValue == (int)Friendly_Hover)
        return Friendly_Hover_Out_Color;
    else if ((int)StencilValue == (int)Friendly_Select)
        return Friendly_Select_Out_Color;
    else if ((int)StencilValue == (int)Enemy_Hover)
        return Enemy_Hover_Out_Color;
    else if ((int)StencilValue == (int)Enemy_Select)
        return Enemy_Hover_Out_Color;
    else
        return DefaultSceneColor;

    if ((int)StencilValue == (int)Stencil_1)
        return Stencil_Out_Color_1;
    else if ((int)StencilValue == (int)Stencil_2)
        return Stencil_Out_Color_2;
    else if ((int)StencilValue == (int)Stencil_3)
        return Stencil_Out_Color_3;
    else if ((int)StencilValue == (int)Stencil_4)
        return Stencil_Out_Color_4;
    else
        return DefaultSceneColor;

    if (Center != input01)
        return 1;
    if (Center != input02)
        return 1;
    if (Center != input03)
        return 1;
    if (Center != input04)
        return 1;
    if (Center != input05)
        return 1;
    if (Center != input06)
        return 1;
    if (Center != input07)
        return 1;
    if (Center != input08)
        return 1;
    else
        return 0;

    float stencil_center;
    float depth_center;
    float stencil_input01;
    float depth_input01;
    float stencil_input02;
    float depth_input02;
    float stencil_input03;
    float depth_input03;
    float stencil_input04;
    float depth_input04;
    float stencil_input05;
    float depth_input05;
    float stencil_input06;
    float depth_input06;
    float stencil_input07;
    float depth_input07;
    float stencil_input08;
    float depth_input08;

//주변부랑 스텐실 번호가 다르면, 외곽라인
if (stencil_center != stencil_input01)
{
    //스텐실 번호가 다른 픽셀의 커스텀 뎁스 크기가 작다 = 앞에 있는 물체다 = 그리지 않음.
    //뒤쪽에 가려져 있는 경우는 그냥 스텐실 번호 넣고, 외곽선을 안그리면 됨.
    //하지만, 그러면 물체가 서로 접해있는 라인에 외곽선이 그려짐.(내부로 파고드는 물체 주변에 외곽선 잡힘.)
    //이걸 방지하기 위함.
    if (depth_center > depth_input01)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input02)
{
    if (depth_center > depth_input02)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input03)
{
    if (depth_center > depth_input03)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input04)
{
    if (depth_center > depth_input04)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input05)
{
    if (depth_center > depth_input05)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input06)
{
    if (depth_center > depth_input06)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input07)
{
    if (depth_center > depth_input07)
        return 0;
    else
        return 1;
}
else if (stencil_center != stencil_input08)
{
    if (depth_center > depth_input08)
        return 0;
    else
        return 1;
}
else
    return 0;
}