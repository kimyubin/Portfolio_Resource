using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RJump : IState
{
    private PlayerScript Player;
    const float defAngle = 30;
    const float defXpos = -0.15f;

    public override void Enter(PlayerScript Player)
    {
        this.Player = Player;

    }
    public override void SUpdate()
    {
        if((Input.GetAxisRaw("Horizontal") > 0)) // 우클릭
        {
            if(Player.feetChecker.CheckGround())//점프X
            {
                Player.SetState(new RMove());
            }
            else//점프O
            {
                //계속 진행
            }
        }
        else if((Input.GetAxisRaw("Horizontal") < 0)) // 좌클릭
        {
            
            if(Player.feetChecker.CheckGround())//점프X
            {
                Player.SetState(new LMove());
            }
            else//점프O
            {
                Player.SetState(new LJump());
            }
        }
        else
        {
            if (!Player.feetChecker.CheckGround())//점프만
            {
                Player.SetState(new JumpIdle());
            }
            if (Player.feetChecker.CheckGround())//완전 idle
            {
                Player.SetState(new IdleState());
            }

        }

        FuntionUpdate();

    }
    public override void Exit()
    {
        if(Player.Body.transform.eulerAngles.z < 180) //양수
            IState.preBodyAngle = Player.Body.transform.eulerAngles.z;
        else//음수
            IState.preBodyAngle = Player.Body.transform.eulerAngles.z - 360;

        IState.preHeadPos = Player.circleHead.transform.localPosition;
        IState.deltaAngle = 0;
    }
    public override void FuntionUpdate()
    {
        if(Player.Body.transform.eulerAngles.z == defAngle)
            return;
        
        float moveAnimTime = Mathf.Abs((defAngle-IState.preBodyAngle)/20f) * Player.refAnimTime;//(20도 당 1초)
        
        if(IState.deltaAngle < moveAnimTime)
        {
            IState.deltaAngle += Time.deltaTime;
            if(IState.deltaAngle >= moveAnimTime)
                IState.deltaAngle = moveAnimTime;
        }
        if(moveAnimTime == 0)
            return;
            
        float refTrigon = Mathf.Sin(IState.deltaAngle/moveAnimTime * 0.5f * Mathf.PI);

        //몸 기울기
        float rotatinTri = (refTrigon * (defAngle - IState.preBodyAngle)) + IState.preBodyAngle;
        Player.Body.transform.rotation = Quaternion.Euler(new Vector3(0,0,rotatinTri));

        //머리 x축
        float moveHeadXpos = (refTrigon * (defXpos - IState.preHeadPos.x)) + IState.preHeadPos.x;
        Player.circleHead.transform.localPosition = new Vector3(moveHeadXpos,Player.circleHead.transform.localPosition.y,Player.circleHead.transform.localPosition.z);
    }

    
}
