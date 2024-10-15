using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JumpBehaviour : HeroBehaviour
{
    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (animator.GetBool("JumpIn"))
        {
            Vector2 newV = rigid.velocity;
            newV.y = jumpPower;
            rigid.velocity = newV;
            animator.SetBool("IsJumping", true);
        }
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        int moveV = animator.GetInteger("HorizontalIn");
        if(moveV != 0)
            rigid.transform.localScale = new Vector3(moveV, 1, 1);
        rigid.transform.Translate(Vector3.right * moveV * Time.deltaTime * movePower, Space.World);
    }

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        
    }

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
