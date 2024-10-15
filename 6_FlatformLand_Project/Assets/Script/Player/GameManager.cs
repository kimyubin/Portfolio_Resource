using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public enum Layer: int{PlayerLayer = 8, PlayerFootLayer=9 , Raycast = 10,RaycastIgnore = 11 , PlayerIgnore = 14, PlayerBody = 15}
    public GameObject PlayerGroup;
    public GameObject Playerchar;
    public GameObject DeadBody;
    static bool dieTrig;
    static int deadCount;
    

    // Start is called before the first frame update
    void Start()
    {
        PhysicsIgnore();
        dieTrig = false;
    }

    // Update is called once per frame
    void Update()
    {
        DieFunc();
    }

    void PhysicsIgnore()
    {
        Physics2D.IgnoreLayerCollision((int)Layer.PlayerIgnore,(int)Layer.Raycast,true);
        Physics2D.IgnoreLayerCollision((int)Layer.PlayerIgnore,(int)Layer.RaycastIgnore,true);

        Physics2D.IgnoreLayerCollision((int)Layer.PlayerLayer,(int)Layer.PlayerFootLayer,true);
        Physics2D.IgnoreLayerCollision((int)Layer.PlayerLayer,(int)Layer.PlayerIgnore,true);
        Physics2D.IgnoreLayerCollision((int)Layer.PlayerBody,(int)Layer.PlayerIgnore,true);
        Physics2D.IgnoreLayerCollision((int)Layer.PlayerLayer,(int)Layer.PlayerFootLayer,true);

    }

    void DieFunc()
    {
        if(dieTrig)
        {
            dieTrig = false;
            Playerchar.GetComponent<PlayerScript>().PlayerDeadCopy();
            Instantiate(DeadBody, transform.position, transform.rotation);            
            Playerchar.transform.localPosition = new Vector3(0,0,0);
        }           
    }

    public static void DieFuncStatic()
    {
        dieTrig = true;
    }
}
