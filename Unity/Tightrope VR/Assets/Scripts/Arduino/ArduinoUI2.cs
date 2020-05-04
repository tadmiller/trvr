/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * This module updates the UI in the Oculus Rift
 *
 */

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Threading;
using UnityEngine.Networking;
using System;

public class ArduinoUI2 : MonoBehaviour
{
	public Text ArduinoDebug;
	public Image level;
	public Image footIcon;
	public Image leanLeft;
	public Image leanRight;
	public Text Stats;
	public Sprite footHTTPError;
	public Sprite footParseError;
	public Sprite footFlat;
	public Sprite footToes;
	public Sprite footHeels;
	public Sprite footNoop;
	public Sprite arrowLeft;
	public Sprite arrowRight;

	private int cycles = 0;
	private int errCycles = 0;
	private int totalCycles = 0;
	private ArduinoHelpers.Lean lean;
	private int leanLeftCycles = 0;
	private int leanRightCycles = 0;

	void Start()
    {
		ArduinoHelpers.noop = new ArduinoDataSet(new ArduinoData(0, 0, 0, 0), new ArduinoData(0, 0, 0, 0));
		ArduinoHelpers.flat = new ArduinoDataSet(new ArduinoData(800, 800, 800, 800), new ArduinoData(800, 800, 800, 800));
		ArduinoHelpers.heels = new ArduinoDataSet(new ArduinoData(0, 0, 0, 800), new ArduinoData(0, 0, 0, 800));
		ArduinoHelpers.toes = new ArduinoDataSet(new ArduinoData(800, 800, 800, 0), new ArduinoData(800, 800, 800, 0));
		// ArduinoDebug.enabled = false;
		Stats.enabled = false;
		ArduinoHelpers.HintsUsed = 0;
		lean = ArduinoHelpers.Lean.None;
		HideLeans();
        StartCoroutine(UpdateUI());

		ArduinoHelpers.OpenDataStream();
    }

	// Hides the foot icon
	void HideUI()
	{
		footIcon.enabled = false;
	}

	// Hide the leaning UI
	void HideLeans()
	{
		leanLeft.enabled = false;
		leanRight.enabled = false;
	}

	// Shows the foot icon
	void ShowUI()
	{
		footIcon.enabled = true;
	}

	// Hide the level name
	void HideLevel()
	{
		level.enabled = false;
	}

	// Gets Arduino data over HTTP
	public IEnumerator ResolveData(ArduinoDataSet data)
	{
		StartCoroutine(ArduinoHelpers.GetArduinoDataSerial((value, httpStatus) => {
			data.left = value;
			data.errL = httpStatus;
			data.right = value;
			data.errR = httpStatus;
		}));

		/*StartCoroutine(ArduinoHelpers.GetArduinoData("http://192.168.1.177/data", (value, httpStatus) => {
			data.left = value;
			data.errL = httpStatus;
		}));

		StartCoroutine(ArduinoHelpers.GetArduinoData("http://192.168.1.241/data", (value, httpStatus) => {
			data.right = value;
			data.errR = httpStatus;
		}));*/

		Debug.Log(data.errR);

		yield return new WaitUntil(() => data.errL != ArduinoHelpers.StatusCode.Noop && data.errR != ArduinoHelpers.StatusCode.Noop);
	}
	
	// Updates level stats
	void UpdateStats()
	{
		String stats = "Level Stats\n" +
					   "------------------\n" +
						"Hints Used\n" + ArduinoHelpers.HintsUsed +
						"\n------------------\n" +
						"Suggested Level\n" + (ArduinoHelpers.HintsUsed > 20 ? "Easy" : (ArduinoHelpers.HintsUsed > 10 ? "Medium" : "Hard"));

		Stats.text = stats;
	}

	// Pulls data from different resources and uses this to update the UI
	IEnumerator UpdateUI()
    {
        while (true)
        {
			UpdateStats();

			ArduinoDataSet data = new ArduinoDataSet();

			yield return StartCoroutine(ResolveData(data));

			ArduinoHelpers.last = data;

            // If one of the Arduino's doesn't return data, perform a copy from the data of one foot to the next
            if (data.errL == ArduinoHelpers.StatusCode.Success && data.errR != ArduinoHelpers.StatusCode.Success)
                data.right = data.left;
            else if (data.errR == ArduinoHelpers.StatusCode.Success && data.errL != ArduinoHelpers.StatusCode.Success)
                data.left = data.right;

			// If the data is successful
            if (data.errL == ArduinoHelpers.StatusCode.Success || data.errR == ArduinoHelpers.StatusCode.Success)
			{
				errCycles = 0;
				cycles++;
				totalCycles++;

				// Cycles is the amount of successful refreshes on the UI
				// If we are continuously successful, hide the UI
				if (cycles > 30 && cycles < 60)
					HideUI();

				if (totalCycles > 30 && totalCycles < 60)
					HideLevel();

				DetermineStance(data);
			}
			// If there is an error, do something about it
			else if (data.errL != ArduinoHelpers.StatusCode.Noop && data.errR != ArduinoHelpers.StatusCode.Noop)
			{
				errCycles++;

				if (errCycles > 3)
				{
					cycles = 0;

					// Show the UI
					ShowUI();
					HideLeans();
					
					// If the error is associated with HTTP, show an HTTP error icon
					if (data.errL == ArduinoHelpers.StatusCode.HTTPErr && data.errR == ArduinoHelpers.StatusCode.HTTPErr)
						footIcon.sprite = footHTTPError;
					// If it's associated with parsing JSON, show a JSON error icon
					else if (data.errL == ArduinoHelpers.StatusCode.ParseErr && data.errR == ArduinoHelpers.StatusCode.ParseErr)
						footIcon.sprite = footParseError;
				}
			}

			yield return new WaitForSeconds(0.1F);
		}
	}

	// Determines the stance of the foot and updates the UI accordingly
	private void DetermineStance(ArduinoDataSet data)
	{
		// Determine the icon to show
		if (data.IsHeels())
			footIcon.sprite = footHeels;
		else if (data.IsToes())
			footIcon.sprite = footToes;
		else if (data.IsNoop())
			footIcon.sprite = footNoop;
		else
			footIcon.sprite = footFlat;

		// Check to see if we're leaning too far to one direction
		lean = data.IsLeaning();

		// If it's the case, start counting for the direction
		if (lean == ArduinoHelpers.Lean.Left)
				leanLeftCycles++;
		else if (lean == ArduinoHelpers.Lean.Right)
				leanRightCycles++;
		
		// Reset if we're not leaning
		if (lean == ArduinoHelpers.Lean.None)
		{
			//Debug.Log("RESET");
			leanLeftCycles = 0;
			leanRightCycles = 0;
			HideLeans();
		}

		// Check to see if we've been leaning for enough time (aka not a false positive) and tell the user to stop
        // Set to 0 - can be tweaked to make it more or less sensitive
		if (leanLeftCycles > 1 || OVRGearVrControllerTest.DEBUG1)
		{
			ArduinoHelpers.HintsUsed++;

			leanRight.enabled = true;
            leanLeft.enabled = false;
		}

		if (leanRightCycles > 1 || OVRGearVrControllerTest.DEBUG2)
		{
			ArduinoHelpers.HintsUsed++;

			leanLeft.enabled = true;
            leanRight.enabled = false;
		}
	}
}
