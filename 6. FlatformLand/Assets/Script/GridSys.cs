using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GridSys : MonoBehaviour
{
    public enum blockNum
    {
        empty,
        quad,
        tir26,
        tri45,
        tri63
    }
    static int blockState;
    public GameObject Quad;
    public GameObject Tri26;
    public GameObject Tri45;
    public GameObject Tri63;
    
    public GameObject QuadColl;
    public GameObject Tri26Coll;
    public GameObject Tri45Coll;
    public GameObject Tri63Coll;
    

    static int width, height, cellsize ;
    Grid gridObj;



    // Start is called before the first frame update
    void Start()
    {
        width = 100;
        height = 100;
        cellsize = 2;
        
        gridObj = new Grid(width,height,cellsize);
        blockState = (int)blockNum.empty;

        
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetMouseButtonDown(0))
        {
            if(Input.mousePosition.y < 80)
                return;
            if(Input.mousePosition.y > 1000)
                return;          


            gridObj.SetValue(GetMousePos(), 56);
            Debug.Log(GetMousePos().x + "+" + GetMousePos().y);
        }

    }
    public static Vector3 GetMousePos()
    {
        Vector3 mouseV3 = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        mouseV3.z = 0f;
        return mouseV3;
    }
    public static void GetXY(Vector3 worldPosition, out int x, out int y)
    {
        x = Mathf.FloorToInt(worldPosition.x / cellsize);
        y = Mathf.FloorToInt(worldPosition.y / cellsize);
    }
    public static Vector3 GetRealPos(Vector3 worldPosition)
    {
        //cellsize로 나눠서 1,2,3 증가하기 때문에 Array 번호는 매길수 있지만, 실제 위치는 *2+1을 해야함.
        //*2는 cellsize, +1은 홀수열
        return new Vector3(Mathf.FloorToInt(worldPosition.x / cellsize) * cellsize + 1, Mathf.FloorToInt(worldPosition.y / cellsize) * cellsize + 1, 0);
    }
    public static int GetBlockNum()
    {
        return blockState;
    }
    public static void SetBlockNum(int n)
    {
        blockState = n;
    }

}
public class Grid
{
    private int width, height;
    private float cellsize;
    public int[,] gridArr;
    
    public Grid(int w, int h, float cz)
    {
        this.width = w;
        this.height = h;
        this.cellsize = cz;

        gridArr = new int[width, height];
        
        for (int i = 0; i< gridArr.GetLength(0); i++)
        {
            for (int j = 0; j< gridArr.GetLength(1); j++)
            {
                float x = i;
                float y = j;
                //Debug.DrawLine(GetMapPosition(x,y),GetMapPosition(x,y+1), Color.white, 1000f);
                //Debug.DrawLine(GetMapPosition(x,y),GetMapPosition(x+1,y), Color.white, 1000f);
            }
        }
        //Debug.DrawLine(GetMapPosition(0, height),GetMapPosition(width,height), Color.white, 1000f);
        //Debug.DrawLine(GetMapPosition(width, 0),GetMapPosition(width,height), Color.white, 1000f);
    }
    Vector3 GetMapPosition(float x, float y)
    {
        return new Vector3(x,y) * cellsize;
    }
    
    public void SetValue(int x, int y, int value)
    {
        if (x >= 0 && y >= 0 && x < width && y < height)
            gridArr[x, y] = value;
        else
            return;//범위 밖 포인트 그냥 리턴
        Debug.Log(x + "," + y + "," + gridArr[x, y]);

    }
    public void SetValue(Vector3 worldPosition, int value)
    {
        int x,y;
        //위치를 정수단위로 끊음. 
        //cellsize로 나눠서 1,2,3 증가하기 때문에 Array 번호는 매길수 있지만, 실제 위치는 *2+1을 해야함.
        GridSys.GetXY(worldPosition, out x, out y);
        SetValue(x, y, value);
    }
}
