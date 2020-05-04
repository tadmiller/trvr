/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * Used to align position of human character model with VR Camera as well as spawn footprints and coordinate animation cycles based on speed of movement.
 *
 */

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.VR;
using System;

public class OVRCharacter : MonoBehaviour {

	public GameObject footsteps;
	public GameObject dil;
	public GameObject mainCamera;
    public GameObject centerEye;
    private Animator animator;
    private Vector3 lastPos;
	private int count;

    public float moveFactor = 10f;


	// Use this for initialization
	void Start () {
        animator = GetComponent<Animator>();
		count = 0;
	}
	
	// Update is called once per frame
	void Update () {
        // calculate position of character based on oculus position
        var eyePos = centerEye.transform.position;
        var pos = transform.position;
        float cameraY = centerEye.transform.rotation.eulerAngles.y;
        pos.x = centerEye.transform.position.x - (0.3f * Mathf.Sin((cameraY * Mathf.PI / 180)));
        pos.z = centerEye.transform.position.z - (0.3f * Mathf.Cos((cameraY * Mathf.PI / 180)));

        // dir will be used to move the body slightly back based on the direction of the oculus
        var dir = centerEye.transform.forward;
        pos.y = eyePos.y - 2.0f; // more negative is up

		Vector3 tracking = UnityEngine.XR.InputTracking.GetLocalPosition(UnityEngine.XR.XRNode.CenterEye);
		tracking.y = dil.transform.position.y;
		tracking.x *= 3;
		tracking.z *= 3;

		// apply position to character
		transform.position = pos; // - (dir * 0.2f);

        // calculate how much and which direction we're moving
        float vSpeed = transform.position.z - lastPos.z;
        float hSpeed = transform.position.x - lastPos.x;

        // save last position
        lastPos = transform.position;

        // apply movement to animation (move factor is used to raise the values to activate animations)
        animator.SetFloat("VSpeed", vSpeed * moveFactor);
        animator.SetFloat("HSpeed", hSpeed * moveFactor);

		// apply oculus rotation in the y axis to the character
		var camRot = centerEye.transform.rotation.eulerAngles;
		var rot = transform.rotation.eulerAngles;
		rot.y = camRot.y;
		rot.x = 0;
		rot.z = 0;
		transform.rotation = Quaternion.Euler(rot);

		count = (count + 1) % 51;

		// high is less sensitive
		if (Mathf.Abs(vSpeed) < 0.001f)
        {
            animator.SetTrigger("Idle");
            animator.SetFloat("VSpeed", 0);

			return;
        }

		if (count == 50)
			StartCoroutine(SpawnFootsteps(rot));
    }

	private IEnumerator SpawnFootsteps(Vector3 rot)
	{
		GameObject spawn = Instantiate(footsteps); //creates an instance of footsteps graphic asset
		spawn.transform.position = transform.position; //puts the location of the footsteps instantiated as the same position of the human model feet

		rot.x = 90;
		spawn.transform.rotation = Quaternion.Euler(rot); //puts the rotation of the footsteps instantiated as the same rotation of the human model feet

        Destroy(spawn, 2); //destroy these instantiations so that they can continue with next step locations
		yield break;
	}
}
