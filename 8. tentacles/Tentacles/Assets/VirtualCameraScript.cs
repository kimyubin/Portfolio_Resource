using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.PostProcessing;

public class VirtualCameraScript : MonoBehaviour
{
    IEnumerator currentCoroutine;
    PostProcessingBehaviour postProc;
    // Start is called before the first frame update
    void Start()
    {
        currentCoroutine = null;
        postProc = GameObject.Find("Camera").GetComponent<PostProcessingBehaviour>();
    }

    // Update is called once per frame
    void Update()
    {
    }


    // 일정시간(delay)에 걸쳐 OrthoIn만큼의 줌인/줌아웃을 수행하는 코루틴을 실행
    // 최대 1개의 코루틴 실행만을 허용 - 새 줌인/줌아웃 명령이 들어올 경우 이전의 코루틴을 종료
    public void DeleyFunc(Cinemachine.CinemachineVirtualCamera vcamin, float OrthoIn, float delay)
    {   
        if(currentCoroutine != null)StopCoroutine(currentCoroutine); 
        currentCoroutine = DelayCameraZoom(vcamin, OrthoIn, delay);
        StartCoroutine(currentCoroutine);
    }
    
    IEnumerator DelayCameraZoom(Cinemachine.CinemachineVirtualCamera vcam, float targetOrtho, float delay)
    {
        //속도조절용.
        float OrthoOrigin = vcam.m_Lens.OrthographicSize;
        float OrthoPer = (targetOrtho - vcam.m_Lens.OrthographicSize);
        float time = 0;
        float coEfficient = Mathf.PI * 0.5f / delay; //0 ~ delay 사이의 시간을 0 ~ 1/2 PI로 변환하기 위한 계수
        
        //목표 시야각까지 단계별로 확대
        //차지 공격 시 시간이 느려지기 때문에 unscaledDeltaTime 사용.
        while(time < delay) 
        { 
            yield return null;
            time += Time.unscaledDeltaTime;
            //줌인, 줌아웃이 끝날 때 좀더 부드럽게 보이기 위해 종단기울기가 0인 sin함수 적용 (0 ~ 1/2 PI)
            vcam.m_Lens.OrthographicSize = OrthoOrigin + (Mathf.Pow(Mathf.Sin(time * coEfficient), 0.5f) * OrthoPer);
        }

    }

    public void SetBlur(bool value)
    {
        postProc.profile.motionBlur.enabled = value;
    }
}
