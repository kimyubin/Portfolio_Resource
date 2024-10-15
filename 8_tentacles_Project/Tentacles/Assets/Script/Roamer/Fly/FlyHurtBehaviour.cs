using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FlyHurtBehaviour : StateMachineBehaviour
{
    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    float origin;
    Vector3 moveX;
    
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (animator.GetBool("knockbackRL"))
        {
            animator.transform.rotation = Quaternion.Euler(0, 180, 0);
        }
        else
        {
            animator.transform.rotation = Quaternion.Euler(0, 0, 0);
        }
        moveX = new Vector3(-stateInfo.length, 0, 0);
        origin = animator.transform.position.x;
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (moveX.x < 0)
        {
            animator.transform.Translate(moveX * Time.deltaTime); //Space.World인자를 안주었으므로, 현재 보는 방향의 반대쪽으로 이동
            moveX.x += Time.deltaTime;  //넉백 중 넉백속도가 시간에 대해 선형으로 감소
        }
    }

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    //override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    
    //}

    // OnStateMove is called right after Animator.OnAnimatorMove()
    //override public void OnStateMove(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    // Implement code that processes and affects root motion
    //}

    // OnStateIK is called right after Animator.OnAnimatorIK()
    //override public void OnStateIK(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    // Implement code that sets up animation IK (inverse kinematics)
    //}
}
