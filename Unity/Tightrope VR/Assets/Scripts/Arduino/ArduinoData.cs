/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * Stores a reading for Arduino pressure
 *
 */

using System;

public class ArduinoData
{
	public int back, front, midl, midr;

	public ArduinoData(int front, int midl, int midr, int back)
	{
		this.front = front;
		this.midl = midl;
		this.midr = midr;
		this.back = back;
	}

	// Check and see if two ArduinoDatas are -approximately- equal
	public bool ApproxEquals(ArduinoData data, int err)
	{
		if (Math.Abs(this.front - data.front) < err && Math.Abs(this.midl - data.midl) < err && Math.Abs(this.back - data.back) < err)
			return true;

		return false;
	}

	// Check to see if either data set has an error
	public bool IsErr()
	{
		if (this.front == -1 && this.midl == -3 && this.midr == -3 && this.back == -7)
			return true;

		return false;
	}
}