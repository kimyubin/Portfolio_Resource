using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class IState
{
    public static float preBodyAngle=0;
    public static float deltaAngle = 0;
    public static Vector3 preHeadPos;

    public abstract void Enter(PlayerScript Player);
    public abstract void SUpdate();
    public abstract void Exit();
    public abstract void FuntionUpdate();
    
}
