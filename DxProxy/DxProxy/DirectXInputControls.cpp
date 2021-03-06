#include "DirectXInputControls.h"
#include <assert.h>
#include <VireioUtil.h>

using namespace vireio;


DirectXInputControls::DirectXInputControls()
{
	Reset();
}

DirectXInputControls::~DirectXInputControls()
{
}

void DirectXInputControls::UpdateInputs()
{
	prevState = currentState;
	for(int ii=0; ii<256; ii++)
		prevState.keyState[ii] = currentState.keyState[ii];
	prevState.xInputState = currentState.xInputState;
	
	// Get the keyboard state
	for(int ii=0; ii<256; ii++)
		currentState.keyState[ii] = GetAsyncKeyState(ii)?1:0;
	
	// Get the gamepad state
	DWORD Result = XInputGetState(NULL, &currentState.xInputState);

	if(Result == ERROR_SUCCESS)
	{
		// Unpack buttons from bitfields to bools
		for(DWORD i = 0; i < 16; ++i)
		{
			if((currentState.xInputState.Gamepad.wButtons >> i) & 1)
				currentState.xButtonsStatus[i] = true;
			else
				currentState.xButtonsStatus[i] = false;
		}
	}
}

void DirectXInputControls::Reset()
{
	debugf("Reset DirectXInputControls\n");
	prevState.Reset();
	currentState.Reset();
}

InputControlState *DirectXInputControls::GetCurrentState()
{
	return &currentState;
}

InputControlState *DirectXInputControls::GetPreviousState()
{
	return &prevState;
}


DirectXInputControlState::DirectXInputControlState()
{
	Reset();
}

void DirectXInputControlState::Reset()
{
	for(int ii=0; ii<256; ii++)
		keyState[ii] = 0;
	for(int ii=0; ii<16; ii++)
		xButtonsStatus[ii] = 0;
	ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
}

bool DirectXInputControlState::GetKeyState(int virtualKeyCode)
{
	//if(virtualKeyCode<0 || virtualKeyCode>255)
	//	return false;
	return (keyState[virtualKeyCode]) ? true : false;
}

bool DirectXInputControlState::GetButtonState(int button)
{
	if(button<0 || button>=16)
		return false;
	return xButtonsStatus[button];
}

float DirectXInputControlState::GetAxis(InputControls::GamepadAxis axis)
{
	int axisPosition;
	
	// Get axis position
	switch(axis) {
	case InputControls::GamepadAxis::LeftStickX:  axisPosition = xInputState.Gamepad.sThumbLX; break;
	case InputControls::GamepadAxis::LeftStickY:  axisPosition = xInputState.Gamepad.sThumbLY; break;
	case InputControls::GamepadAxis::RightStickX: axisPosition = xInputState.Gamepad.sThumbRX; break;
	case InputControls::GamepadAxis::RightStickY: axisPosition = xInputState.Gamepad.sThumbRY; break;
	default: assert(0); return 0;
	}
	
	// Normalize to [-1,1]
	return (float)axisPosition / 32768;
}
