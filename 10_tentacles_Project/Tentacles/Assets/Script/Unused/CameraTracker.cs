using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraTracker : MonoBehaviour
{
    PlayerCharacter playerCharacter;
    public float widthMax;
    public float heightMax;

    // Start is called before the first frame update
    void Start()
    {
        playerCharacter = GetComponentInParent<Player>().GetComponentInChildren<PlayerCharacter>();
    }

    // Update is called once per frame
    void Update()
    {
        
        Vector3 playerPosition = playerCharacter.transform.position;
        Vector3 newPosition = playerPosition - transform.position;
        
        if (newPosition.x > widthMax) newPosition.x = widthMax;
        if (newPosition.x < -widthMax) newPosition.x = -widthMax;

        if (newPosition.y > heightMax) newPosition.y = heightMax;
        if (newPosition.y < -heightMax) newPosition.y = -heightMax;
        newPosition = playerPosition - newPosition;
        
        newPosition.z = -10;
        transform.position = newPosition;
    }
}
