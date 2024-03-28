using System.Collections;
using System.Collections.Generic;
using UnityEngine;
 public class grapplingHook : MonoBehaviour 
 {
 
     Camera cam;
     Rigidbody rb;
 
     RaycastHit grapplePoint;
 
 
     bool isGrappling = false;
 
     //keeps track of the lenght of your "rope"
     float distance;
 
     //lets you control how fast you want your grappling hook to be
     public float grappleSpeed = 5f;
 
     void Start () {
 
         // Get Camera and Rigidbody
         cam = Camera.main;
         rb = GetComponent<Rigidbody>();
     }
     
     void Update () {
         // ray from camera into the scene
         Ray ray = cam.ScreenPointToRay(Input.mousePosition);
 
 
         // Check if a button is pressed and if the Raycast hits something
         if (Input.GetButtonDown("Fire1") && Physics.Raycast(ray, out grapplePoint))
         {
             isGrappling = true;
             Vector3 grappleDirection = (grapplePoint.point - transform.position);
             rb.velocity = grappleDirection.normalized * grappleSpeed;//This will instantly accelerate the player towards the grappling point
         }
 
         //turn grappling mode off when the button is released
         if (Input.GetButtonUp("Fire1"))
             isGrappling = false;
 
 
         //when in grappling mode (Thats when the magic happens :D)
         if (isGrappling)
         {
             //Look at the object you are currently grappling. Not really necessary but it looks cool
             transform.LookAt(grapplePoint.point);
 
             //Get Vector between player and grappling point
             Vector3 grappleDirection = (grapplePoint.point - transform.position);
 
 
             if (distance < grappleDirection.magnitude)// With this you can determine if you are overshooting your target. You are basically checking, if you are further away from your target then during the last frame
             {
                 float velocity = rb.velocity.magnitude;//How fast you are currently
 
                 Vector3 newDirection = Vector3.ProjectOnPlane(rb.velocity, grappleDirection);//So this is a bit more complicated
                 //basically I am using the grappleDirection Vector as a normal vector of a plane.
                 //I am really bad at explaining it. Partly due to my bad english but it is best if you just look up what Vector3.ProjectOnPlane does.
 
                 rb.velocity = newDirection.normalized * velocity;//Now I just have to redirect the velocity
 
             }
             else//So this part is executed when you are grappling towards the grappling point
                 rb.AddForce(grappleDirection.normalized * grappleSpeed);//I use addforce just to keep the velocity rather constant. You can fiddle around with the forcemodes a bit to get better results
 
             //Calculate distance between player and grappling point
             distance = grappleDirection.magnitude;
 
         }
         else
             transform.rotation = Quaternion.LookRotation(Vector3.forward, Vector3.up); //resets the rotation. Only necessary if you used transform.LookAt() before
     }
 }