using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ButtonFunctionality : MonoBehaviour
{

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        // returns true if the primary button (typically “A”) is currently pressed.
        if (OVRInput.Get(OVRInput.Button.One))
        {
            Debug.Log("dil");
        }
    }
}
