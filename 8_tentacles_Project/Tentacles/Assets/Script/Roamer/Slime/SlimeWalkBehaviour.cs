using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SlimeWalkBehaviour : StateMachineBehaviour
{
    Vector3 Move;
    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (animator.GetBool("isOut"))//범위 지정 콜라이더를 벗어난 경우, 콜라이더 안쪽으로 경로 설정
        {
            if (animator.GetFloat("RangeX") - animator.transform.position.x < 0) Move = new Vector3(-animator.GetFloat("moveV"), 0, 0);
            else Move = new Vector3(-animator.GetFloat("moveV"), 0, 0);

            animator.SetBool("isOut", false);
            animator.SetFloat("moveV", Move.x);
        }
        else
            Move = new Vector3(animator.GetFloat("moveV"), 0, 0);
        if (Move.x == 1)
            animator.transform.rotation = Quaternion.Euler(0, 0, 0);
        else
            animator.transform.rotation = Quaternion.Euler(0, 180, 0);
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        animator.transform.Translate(Move * Time.deltaTime, Space.World);
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
