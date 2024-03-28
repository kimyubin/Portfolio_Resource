using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MainCamera : MonoBehaviour
{
    CameraTracker track;
    PlayerCharacter playerCharacter;
    public bool tracking;

    // Start is called before the first frame update
    void Start()
    {
        track = GetComponentInParent<Player>().GetComponentInChildren<CameraTracker>();
        playerCharacter = GetComponentInParent<Player>().GetComponentInChildren<PlayerCharacter>();
    }

    // Update is called once per frame
    void Update()
    {
        if (!tracking) return;
        Vector3 newPosition = track.transform.position;
        newPosition.y += playerCharacter.cameraMoveY(Time.deltaTime);
        //맵끝 제한 추가 필요
        transform.position = newPosition;
    }
    void setTrack(CameraTracker newtrack)
    {
        track = newtrack;
    }
}
