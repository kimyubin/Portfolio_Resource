using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChargeBehaviour : HeroBehaviour
{
    public float chargeLevel;
    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        Time.timeScale = 1 / (chargeLevel * 2);
        Time.fixedDeltaTime = Time.timeScale * 0.02f; // fixedUpdate는 초당 50회.

        switch (chargeLevel)
        {
            case 1:
                vCamScript.DeleyFunc(vcam, 2f, 1);
                break;
            case 2:
                vCamScript.DeleyFunc(vcam, 1.5f, 1);
                break;
            case 3:
                vCamScript.DeleyFunc(vcam, 1f, 2);
                break;
        }
        vCamScript.SetBlur(true); 
    }
    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    //override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{  
    //}

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        Time.timeScale = 1;
        Time.fixedDeltaTime = Time.timeScale * 0.02f; // fixedUpdate는 초당 50회.

        vCamScript.DeleyFunc(vcam, 4.26f, 0.3f);
        vCamScript.SetBlur(false);
    }

    // OnStateMove is called right after Animator.OnAnimatorMove()
    //override public void OnStateMove(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
        
    //}

    // OnStateIK is called right after Animator.OnAnimatorIK()
    //override public void OnStateIK(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    // Implement code that sets up animation IK (inverse kinematics)
    //}
}
