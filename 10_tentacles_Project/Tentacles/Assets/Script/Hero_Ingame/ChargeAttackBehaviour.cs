using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChargeAttackBehaviour : HeroBehaviour
{
    public GameObject prefab;
    GameObject attack;

    Vector3 pos;
    bool checkAttack = false;
    int moveV;

    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        pos = new Vector3(0.9f, -0.1f);
        pos.x *= animator.transform.localScale.x;
        attack = Instantiate(prefab, animator.rootPosition + pos, Quaternion.identity);
        attack.transform.localScale = animator.transform.localScale;
        SuperArmor = true;
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        attack.transform.position = animator.rootPosition + pos;
    }

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        attack.AddComponent<AttackTrigger>().setDamage(animator.GetFloat("damage"));
        Destroy(attack, 1.0f); //공격판정이 일어나기 전 콜라이더가 사라지는걸 방지하기 위해 0.1초 잔류
        Destroy(attack.GetComponent<AttackTrigger>(), 0.1f);
        //checkAttack = false;
        SuperArmor = false;
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
