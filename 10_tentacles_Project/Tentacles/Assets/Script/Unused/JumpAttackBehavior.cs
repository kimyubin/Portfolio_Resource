using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//현재 사용되지 않는 파일
public class JumpAttackBehavior : StateMachineBehaviour
{
    public GameObject prefab;
    GameObject attack;

    Vector3 pos;
    bool checkAttack = false;

    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        pos = new Vector3(0.9f, -0.1f);
        pos.x *= animator.transform.localScale.x;
        attack = Instantiate(prefab, animator.rootPosition + pos, Quaternion.identity);
        attack.transform.localScale = animator.transform.localScale;
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        attack.transform.position = animator.rootPosition + pos;
        if (checkAttack)
        {
            if (stateInfo.normalizedTime > 0.4)
                Destroy(attack.GetComponent<AttackTrigger>());
        }
        else if (stateInfo.normalizedTime > 0.3)
        {
            checkAttack = true;
            attack.AddComponent<AttackTrigger>().setDamage(animator.GetFloat("damage"));
        }
    }

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        animator.SetInteger("state", 5);
        Destroy(attack);
        checkAttack = false;
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
