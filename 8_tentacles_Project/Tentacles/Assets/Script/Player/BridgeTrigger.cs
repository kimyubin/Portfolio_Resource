using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BridgeTrigger : MonoBehaviour
{
    // Start is called before the first frame update
	void OnTriggerStay2D(Collider2D other)
	{
		if(other.gameObject.tag == "Bridge")
            gameObject.transform.parent.GetComponent<PlayerCharacter>().SetOnBridge(true);

	}
	void OnTriggerExit2D(Collider2D other)
	{
		//브릿지에 닿지 않거나, 통과하면 다시 불통과 상태
		if(other.gameObject.tag == "Bridge")
		{
            gameObject.transform.parent.GetComponent<PlayerCharacter>().SetOnBridge(false);
			Physics2D.IgnoreLayerCollision(8,10,false);
		}
			
	}
}
