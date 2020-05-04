/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * Used to handle inputs and user interface changes during tutorial/calibration stage.
 *
 */

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class TextChanger : MonoBehaviour
{
	private enum STAGE { TUT1, TUT2, TUT3, FLAT, HEELS, TOES, LEFT, RIGHT, END, LD };
	public Text Instructions;
	public Text HoldX;
	public Image Background;
	public Image LoadingBar;
	public string message;
	private static float counter = 0;
	private bool next = false;
	private STAGE stage = STAGE.TUT1;
	private bool reading = false;
	private bool readDone = false;
	private bool readErr = false;

    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(InstructionRoutine());
    }

	IEnumerator TakeCalibration()
	{
		reading = true;

		yield return new WaitUntil(() => ArduinoHelpers.last != null);

		ArduinoDataSet data = ArduinoHelpers.last;
		yield return new WaitUntil(() => data.errL != ArduinoHelpers.StatusCode.Noop && data.errR != ArduinoHelpers.StatusCode.Noop);

		
		data = new ArduinoDataSet(ArduinoHelpers.last.left, ArduinoHelpers.last.right);

		readDone = true;
		reading = false;

		if (data.left == null || data.right == null) //checks to see if there any data readings at all
		{
			readErr = true;
			Instructions.text = (message + "...Read error");
			// error
			yield break;
		}

		Instructions.text = (message + "...OK");

        //for each stage the last reading from the stage's targeted pressure sensor is recorded
		if (stage == STAGE.FLAT)
			ArduinoHelpers.flat = ArduinoHelpers.last; 
		else if (stage == STAGE.HEELS)
			ArduinoHelpers.heels = ArduinoHelpers.last;
		else if (stage == STAGE.TOES)
			ArduinoHelpers.toes = ArduinoHelpers.last;
		else if (stage == STAGE.LEFT)
			ArduinoHelpers.left = ArduinoHelpers.last;
		else if (stage == STAGE.RIGHT)
			ArduinoHelpers.right = ArduinoHelpers.last;

		Debug.Log("STAGE: " + (stage == STAGE.FLAT ? "FLAT" + ArduinoHelpers.flat.ToString() : (stage == STAGE.HEELS ? "HEELS" + ArduinoHelpers.heels.ToString() : (stage == STAGE.TOES ? "TOES" + ArduinoHelpers.toes.ToString() : "OTHER"))));

		yield break;
	}

    // Update is called once per frame
    void Update()
    {
		if (!next)
		{
			bool x = OVRGearVrControllerTest.x;

			if (x) //if X button is held down
			{
				counter += 4; //counter begins to go up by 4

				if (counter > 25 && stage > STAGE.TUT3 && stage < STAGE.END && readDone == false && reading == false) //timer used to help begin the calibration
					StartCoroutine(TakeCalibration());
			}
			if (!x) //if X button is released then timer and recording of data does not begin
				counter = 0;

			if (counter > 100) //if the counter reaches over 100 then that means the data has been read successfully and the tutorial will continue to the next stage
			{
				next = true;
				stage++;
				counter = 0;
			}

			LoadingBar.fillAmount = counter / 100F; //fills loading bar in UI depending on status of X button/data reading completion
		}
	}

    IEnumerator InstructionRoutine()
    {
		while (true)
		{
			yield return new WaitUntil(() => next == true && reading == false);

			if (readErr)
			{
				// do something

				stage--;
				readErr = false;
			}

            //processing which stage the tutorial is currently in and displays the correct text to the tutorial instruction GameObject/Text UI
			if (stage == STAGE.TUT2)
				message = "Please ensure the foot sensor is on";
			else if (stage == STAGE.TUT3)
				message = "Please copy the stances that follow";
			else if (stage == STAGE.FLAT)
				message = "Stand normally (flat)";
			else if (stage == STAGE.HEELS)
				message = "Stand on your heels";
			else if (stage == STAGE.TOES)
				message = "Stand on your toes";
			else if (stage == STAGE.LEFT)
				message = "Lean to the left";
			else if (stage == STAGE.RIGHT)
				message = "Lean to the right";
			else if (stage == STAGE.END)
			{
				Instructions.text = "Now beginning TightRope Virtual Reality";

                //helps to move the loading bar to the top of the screen (animation effect) after text gets replaced with greeting
				Background.transform.GetComponent<RectTransform>().sizeDelta = new Vector2(190, 150);
				LoadingBar.transform.GetComponent<RectTransform>().sizeDelta = new Vector2(190, 150);
				Vector3 pos = Background.transform.position;
				Vector3 newPos;
				HoldX.enabled = false;
				for (float i = 0; i < 120; i++)
				{
					newPos = new Vector3(pos.x, pos.y + ((i * 0.0073F)), pos.z);
					Background.transform.position = newPos;
					yield return new WaitForSeconds(0.025F);
				}
				message = "Hold X to begin the simulation";

			}
			else if (stage == STAGE.LD) //finally loading the next scene with a prompt for the player
			{
				Instructions.text = "Loading next scene...";
				yield return new WaitForSeconds(2);
				SceneManager.LoadScene("Nature Scene");
			}

			yield return new WaitUntil(() => OVRGearVrControllerTest.x == false);
			next = false;
			readDone = false;

			Instructions.text = message;
		}
    }
}
