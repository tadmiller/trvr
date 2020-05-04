/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * Used to display stats after level completion and to warp player to next scene.
 *
 */

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class WarpPoint : MonoBehaviour
{

    [SerializeField]
    private Transform player;
	public Text stats;
    private bool tping = false;

    private void Start()
    {
        Debug.Log("starting");
        tping = false;
    }

    void OnTriggerEnter(Collider other) //if GObject collides with player tagged GO then show stats
    {
		if (other.CompareTag("Player") && !tping)
		{
            tping = true;
            StartCoroutine(ShowStatsAndWait());
        }
    }

	private IEnumerator ShowStatsAndWait()
	{
        stats.enabled = true; //set stats to enabled to be put into a string
        string txt = stats.text;

        for (int i = 8; i > 0; i--) //show stats at end of level for 8 seconds
        {
            stats.text = txt + "\n(" + i + ")";
            yield return new WaitForSeconds(1);
        }

        if (SceneManager.GetActiveScene().buildIndex == 3)  //load next scene of simulation, if scene is final level then go back to tutorial/title screen
            SceneManager.LoadScene(0);

		SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex + 1);

		yield break;
	}
}
