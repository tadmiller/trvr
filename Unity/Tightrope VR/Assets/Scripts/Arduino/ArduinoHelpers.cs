/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * This module contains scripts to get Arduino data
 *
 */

using System.Collections;
using UnityEngine;
using UnityEngine.Networking;
using System;
using System.IO.Ports;

public static class ArduinoHelpers
{
	// For calibration data
	public static ArduinoDataSet flat, heels, toes, noop, left, right, last;

	// HTTP request status code
	public enum StatusCode { Noop, HTTPErr, ParseErr, Success };

	// Direction in which we are leaning
	public enum Lean { None, Left, Right };

	// Count the amount of hints used
	public static int HintsUsed;

	private static SerialPort stream;
	private static bool Opened = false;

	// Use HTTP request to get Arduino data
	public static IEnumerator GetArduinoData(string url, System.Action<ArduinoData, StatusCode> resolve)
	{
		UnityWebRequest www = UnityWebRequest.Get(url);
		www.timeout = 1;
		yield return www.SendWebRequest();

		if (www.isNetworkError || www.isHttpError)
		{
			resolve(null, StatusCode.HTTPErr);

			yield break;
		}
		else
		{
			// Show results as text
			ArduinoData data = null;

			//ArduinoDebug.text = www.downloadHandler.text;

			try
			{
				data = JsonUtility.FromJson<ArduinoData>(www.downloadHandler.text);
			}
			catch
			{
				resolve(null, StatusCode.ParseErr);
				yield break;
			}

			// API Callback
			resolve(data, StatusCode.Success);
		}
	}

	// Opens the serial port
	public static void OpenDataStream()
	{
		Debug.Log("Stream opening");

		if (Opened)
			return;

		stream = new SerialPort("COM5", 9600);
		stream.ReadTimeout = 50;
		stream.Open();
		Opened = true;
	}

	public static IEnumerator GetArduinoDataSerial(System.Action<ArduinoData, StatusCode> resolve)
	{
		DateTime initialTime = DateTime.Now;
		DateTime nowTime;
		TimeSpan diff = default(TimeSpan);

		string dataString = null;
		ArduinoData data = null;

		// A single read attempt
		try
		{
			dataString = null;
			dataString = stream.ReadLine();
			Debug.Log(dataString);
		}
		catch
		{
			dataString = null;
		}

		if (dataString != null)
		{
			if (dataString.Length > 0)
			{
				if (dataString[0] == '{' && dataString[dataString.Length - 1] == '}')
				{
					try
					{
						data = JsonUtility.FromJson<ArduinoData>(dataString);
					}
					catch
					{
						resolve(null, StatusCode.ParseErr);
						yield break;
					}

					resolve(data, StatusCode.Success);
					yield break;
				}
			}

		Debug.Log("parseerr");
			resolve(null, StatusCode.ParseErr);
			yield break;
		}

		Debug.Log("httperr");
		resolve(null, StatusCode.HTTPErr);
		yield break;
	}

	public static void CloseDataStream()
	{
		stream.Close();
	}
}
