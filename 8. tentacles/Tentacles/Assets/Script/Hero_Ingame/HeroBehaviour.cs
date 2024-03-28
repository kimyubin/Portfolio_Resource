using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HeroBehaviour : StateMachineBehaviour
{
    protected static Rigidbody2D rigid;
    protected static float movePower;
    protected static float jumpPower;
    protected static Cinemachine.CinemachineVirtualCamera vcam;
    protected static float cameraYIdle;
    protected static float cameraXIdle;
    protected static float cameraMoveRange;
    protected static VirtualCameraScript vCamScript;
    protected static int attackType;
    private static Animator anim;
    private static bool attackPossible;
    public static bool AttackPossible
    {
        get
        {
            return attackPossible;
        }
        set
        {
            anim.SetBool("AttackPossible", value);
            attackPossible = value;
        }
    }
    private static bool superArmor;
    public static bool SuperArmor
    {
        get
        {
            return superArmor;
        }
        set
        {
            anim.SetBool("SuperArmor", value);
            superArmor = value;
        }
    }

    public static void initBehaviour(PlayerCharacter input)
    {
        anim = input.getAnim();
        rigid = input.getRigid();
        movePower = input.movePower;
        jumpPower = input.jumpPower;
        AttackPossible = attackPossible;
    }
    public static void initCamera(PlayerCharacter input)
    {
        vcam = input.getVcam();
        cameraYIdle = input.cameraYIdle;
        cameraXIdle = input.cameraXIdle;
        cameraMoveRange = input.cameraMoveRange;
        vCamScript = GameObject.Find("VirtualCameraScript").GetComponent<VirtualCameraScript>();
        
    }
}
