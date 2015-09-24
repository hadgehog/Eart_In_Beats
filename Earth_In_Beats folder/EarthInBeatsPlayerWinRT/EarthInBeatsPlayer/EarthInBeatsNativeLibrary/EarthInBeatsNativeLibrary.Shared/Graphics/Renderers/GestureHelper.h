#pragma once

#include <functional>
#include <memory>

ref class GestureHelper sealed{
public:
	GestureHelper();

	void ProcessPress(Windows::UI::Input::PointerPoint ^ppt);
	void ProcessMove(Windows::UI::Input::PointerPoint ^ppt);
	void ProcessRelease(Windows::UI::Input::PointerPoint ^ppt);

internal:
	std::function<void(float x, float y)> Pressed;
	std::function<void(float x, float y)> Released;
	std::function<void(float x, float y)> MoveUpdated;
	std::function<void(float x, float y)> Clicked;
	std::function<void(float scale)> ZoomUpdated;
	std::function<void(float angle)> RotateUpdated;

private:
	Windows::UI::Input::GestureRecognizer ^recognizer;

	// Recognizer handling functions.
	void OnManipulationStarted(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationStartedEventArgs^ e);
	void OnManipulationCompleted(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationCompletedEventArgs ^e);
	void OnManipulationUpdated(Windows::UI::Input::GestureRecognizer^ sender, Windows::UI::Input::ManipulationUpdatedEventArgs^ e);
	void OnTapped(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::TappedEventArgs ^e);
};