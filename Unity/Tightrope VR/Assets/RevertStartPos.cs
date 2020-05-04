using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RevertStartPos : MonoBehaviour
{
    public GameObject TeleportTarget;
    public GameObject OVRCam;
	public GameObject CenterEye;
    [SerializeField]
    private Transform player;

	void OnTriggerEnter(Collider other)
    {
		if (other.CompareTag("Player"))
			OVRCam.transform.position = TeleportTarget.transform.position + new Vector3(CenterEye.transform.localPosition.x, 0, CenterEye.transform.localPosition.z);
    }
}