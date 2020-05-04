/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * ArduinoDataSet contains two ArduinoData objects that store the pressure readings for that cycle
 *
 */

using System;
using UnityEngine;

public class ArduinoDataSet
{
	public ArduinoData left { get; set; }
	public ArduinoData right { get; set; }
	public ArduinoHelpers.StatusCode errL { get; set; }
	public ArduinoHelpers.StatusCode errR { get; set; }
    private const int LEAN_SENSITIVITY = 300;

	public ArduinoDataSet(ArduinoData left, ArduinoData right)
	{
		this.left = left;
		this.right = right;
		errL = ArduinoHelpers.StatusCode.Noop;
		errR = ArduinoHelpers.StatusCode.Noop;
	}

	public ArduinoDataSet() { }

	// Check and see if two values are approximately equal
	private bool ApproxEquals(int one, int two, int err)
	{
		if (Math.Abs(one - two) < err)
			return true;

		return false;
	}

	// Check and see if the user is leaning
	public ArduinoHelpers.Lean IsLeaning()
	{
		/*if ((!ApproxEquals(this.left.midl, this.left.front, LEAN_SENSITIVITY) && this.left.midl > this.left.front) || (!ApproxEquals(this.right.midl, this.right.front, LEAN_SENSITIVITY) && this.right.midl < this.right.front))
        {
			Debug.Log("Leaning left");
			Debug.Log("midl:" + this.left.midl + " back:" + this.left.back + " front:" + this.left.front);
			return ArduinoHelpers.Lean.Left;
        }
        else if ((this.left.midl != 0 && !ApproxEquals(this.right.midl, this.right.front, LEAN_SENSITIVITY) && this.right.midl > this.right.front) || (!ApproxEquals(this.left.midl, this.left.front, LEAN_SENSITIVITY) && this.left.midl < this.left.front))

        {
			Debug.Log("Leaning right");
			Debug.Log("midl:" + this.left.midl + " back:" + this.left.back + " front:" + this.left.front);
			return ArduinoHelpers.Lean.Right;
		}*/

		// leaning right
		if (this.left.midl == 0 && this.left.front > this.left.midl)
		{
			Debug.Log("Leaning right");
			return ArduinoHelpers.Lean.Right;
		}
		else if (this.left.midl > this.left.front && !ApproxEquals(this.left.midl, this.left.front, LEAN_SENSITIVITY))
		{
			Debug.Log("Leaning left");
			return ArduinoHelpers.Lean.Left;
		}

		Debug.Log("Not leaning");

        return ArduinoHelpers.Lean.None;
	}

	// Check and see if the user has flat feet
	public bool IsFlat()
	{
		if (this.left.ApproxEquals(ArduinoHelpers.flat.left, 150) && this.right.ApproxEquals(ArduinoHelpers.flat.right, 150))
			return true;

		return false;
	}

	// Check and see if the user is on their heels
	public bool IsHeels()
	{
		if (this.left.ApproxEquals(ArduinoHelpers.heels.left, 150) && this.right.ApproxEquals(ArduinoHelpers.heels.right, 150))
			return true;

		return false;
	}

	// Check and see if the user is on their toes
	public bool IsToes()
	{
		if (this.left.ApproxEquals(ArduinoHelpers.toes.left, 150) && this.right.ApproxEquals(ArduinoHelpers.toes.right, 150))
			return true;

		return false;
	}

	// Check and see if the shoe isn't on
	public bool IsNoop()
	{
		if (this.left.ApproxEquals(ArduinoHelpers.noop.left, 150) && this.right.ApproxEquals(ArduinoHelpers.noop.right, 150))
			return true;

		return false;
	}

	// Check and see if there is an error with either shoes
	public bool IsErr()
	{
		if (this.left.IsErr() && this.right.IsErr())
			return true;

		return false;
	}

	// Convert the data to a JSON string and return that
	public override String ToString()
	{
		string ret = "";
		if (this.left != null)
			ret += "left:\nfront: " + this.left.front + "\nmidl: " + this.left.midl + "\nmidr: " + this.left.midr + "\nback: " + this.left.back + "\n";
		else
			ret += "left:\nNULL\n";

		if (this.right != null)
			ret += "right:\nfront: " + this.right.front + "\nmidl: " + this.right.midl + "\nmidr: " + this.right.midr + "\nback: " + this.right.back + "\n";
		else
			ret += "right:\nNULL\n";
		return ret;			
	}
}