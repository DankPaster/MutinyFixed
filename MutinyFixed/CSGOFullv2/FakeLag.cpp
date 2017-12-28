#include "FakeLag.h"
#include "Overlay.h"
#include "Netchan.h"
#include "LocalPlayer.h"
#include "FixMove.h"

void FakeLag(int& buttons, uintptr_t FramePointer)
{
#if 0
#ifndef _DEBUG	
	if ((buttons & IN_ATTACK) || (buttons & IN_ATTACK2))
	{
		if (!pSilentChk.Checked)
			WriteByte((ReadInt((uintptr_t)FramePointer) - 0x1C), 1);
	}
	else
#endif
#endif
	{
		int maxChoke = 14;
		if (!NoAntiAimsAreOn(true))
		{
			if (FakeLagDropDown.index == FAKELAG_OFF)
				FakeLagDropDown.index = FAKELAG_STATIC;
			if (FakeLagChokedTxt.iValue == 0)
			{
				FakeLagChokedTxt.iValue = 1;
				ModifyTextboxValue(&FakeLagChokedTxt, 3);
			}
		}

		if (FakeLagDropDown.index == FAKELAG_OFF)
		{
			CurrentUserCmd.bSendPacket = true;
			CurrentUserCmd.iCmdsChoked = 0;
			return;
		}

		int amountToChoke = FakeLagChokedTxt.iValue;

		switch (FakeLagDropDown.index)
		{
			case FAKELAG_STATIC:
			{
				break;
			}
			case FAKELAG_LAGCOMP:
			{
				float speed = LocalPlayer.Entity->GetVelocity().Length2D() * Interfaces::Globals->interval_per_tick;
				int chokeAmmount = 0;
				if (speed < 1)//should be 66/maxChoke instead of 1 but meh
				{
					amountToChoke = maxChoke;
				}
				else
				{
					while (speed * chokeAmmount <= 70)
					{
						//While they aren't breaking lag comp
						chokeAmmount++;
					}
					if (chokeAmmount > maxChoke)
					{
						amountToChoke = maxChoke;
					}
					else
					{//Breaking lag comp, lets make some different
					//values in this area not just minumin
						int minChoke = chokeAmmount;
						int fromMenu = FakeLagChokedTxt.iValue;
						int stepVal = minChoke;
						if (minChoke < fromMenu)
						{
							stepVal = fromMenu;
						}
						static bool switcher = false;
						static int counter = minChoke;
						if(counter < minChoke)
							counter = minChoke;
						if (!CurrentUserCmd.iCmdsChoked)
						{
							switcher = !switcher;
							counter++;
							if (counter > maxChoke)
								counter = minChoke;
						}
						if (switcher)
							amountToChoke = stepVal;
						else
							amountToChoke = counter;

					}
				}
				break;
			}
			case FAKELAG_STEP:
			{
				//steper
				static int toChoke = 2;
				static bool switcher = false;

				if (!CurrentUserCmd.iCmdsChoked)
				{
					switcher = !switcher;
					if (switcher)
						toChoke++;
					if (toChoke > maxChoke)
						toChoke = 2;
				}
				if (switcher)
					amountToChoke = FakeLagChokedTxt.iValue;
				else
					amountToChoke = toChoke;
				break;
			}
			case FAKELAG_RANDOM:
			{
				//random
				int halfval = FakeLagChokedTxt.iValue / 2;
				static int toAdd = halfval + rand() % halfval;
				if (!CurrentUserCmd.iCmdsChoked)
					toAdd = halfval + rand() % halfval;
				amountToChoke = toAdd;
				break;
			}	
		}
		bool OnlyInAir = FakeLagOnlyInAirChk.Checked;
		if (OnlyInAir)
		{
			if (LocalPlayer.Entity->GetFlags() & FL_ONGROUND)
			{
				amountToChoke = 1;
			}
		}
		amountToChoke = clamp(amountToChoke, 1, maxChoke);

		if (CurrentUserCmd.iCmdsChoked < amountToChoke)
		{
			CurrentUserCmd.bSendPacket = false;
			CurrentUserCmd.iCmdsChoked++;
		}
		else
		{
			CurrentUserCmd.bSendPacket = true;
			CurrentUserCmd.iCmdsChoked = 0;
		}
		if (CurrentUserCmd.iCmdsChoked > maxChoke)
		{
			//if you have choked too much make sure it doesnt fuck up
			//and lag player
			CurrentUserCmd.bSendPacket = true;
			CurrentUserCmd.iCmdsChoked = 0;
		}
	}
}