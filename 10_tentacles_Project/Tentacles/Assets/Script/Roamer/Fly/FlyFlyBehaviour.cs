using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FlyFlyBehaviour : StateMachineBehaviour
{
    Vector3 targetLocation; //이동목표지점의 상대위치
    Vector3 move; //목표지점을 향하는 벡터를 정규화한것(크기 1)
    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        targetLocation.x = Random.Range(-10, 6);
        targetLocation.y = Random.Range(-10, 6);
        if (targetLocation.x > -2) targetLocation.x += 4;
        if (targetLocation.y > -2) targetLocation.y += 4;
        targetLocation.Normalize();
        move = targetLocation;
        targetLocation *= 2;
        
        if (targetLocation.x > 0)
            animator.transform.rotation = Quaternion.Euler(0, 0, 0);
        else
            animator.transform.rotation = Quaternion.Euler(0, 180, 0);
        OnStateUpdate(animator, stateInfo, layerIndex); //Update함수 1번 실행을 보장
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (animator.GetBool("isOut"))//범위 지정 콜라이더를 벗어난 경우, 콜라이더 안쪽으로 경로 설정
        {
            if (animator.transform.position.x > animator.GetFloat("RangeX") + animator.GetFloat("RangeW") && targetLocation.x > 0) targetLocation.x *= -1;
            if (animator.transform.position.x < animator.GetFloat("RangeX") - animator.GetFloat("RangeW") && targetLocation.x < 0) targetLocation.x *= -1;
            if (animator.transform.position.y > animator.GetFloat("RangeY") + animator.GetFloat("RangeH") && targetLocation.y > 0) targetLocation.y *= -1;
            if (animator.transform.position.y < animator.GetFloat("RangeY") - animator.GetFloat("RangeH") && targetLocation.y < 0) targetLocation.y *= -1;
            animator.SetBool("isOut", false);
            animator.SetFloat("moveX", targetLocation.x);
            animator.SetFloat("moveY", targetLocation.y);

            if (targetLocation.x > 0)
                animator.transform.rotation = Quaternion.Euler(0, 0, 0);
            else
                animator.transform.rotation = Quaternion.Euler(0, 180, 0);
        }
        animator.transform.Translate(targetLocation * Time.deltaTime, Space.World);
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
