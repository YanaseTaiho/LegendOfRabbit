#include "AnimationTake.h"

using namespace FrameWork;

AnimationTake::AnimationTake()
{
}

AnimationTake::~AnimationTake()
{
	
}

void AnimationTake::AddTakePosition(int frame, const Vector3 & position)
{
	takesPosition.AddTake(frame, position);
}

void AnimationTake::AddTakeScale(int frame, const Vector3 & scale)
{
	takesScale.AddTake(frame, scale);
}

void AnimationTake::AddTakeRotation(int frame, const Quaternion & rotation)
{
	takesRotation.AddTake(frame, rotation);
}

void AnimationTake::DeleteTakePosition(int frame)
{
	takesPosition.DeleteTake(frame);
}

void AnimationTake::DeleteTakeScale(int frame)
{
	takesScale.DeleteTake(frame);
}

void AnimationTake::DeleteTakeRotation(int frame)
{
	takesRotation.DeleteTake(frame);
}

void AnimationTake::UpdateTake(float frame, Vector3 * outPosition, Vector3 * outScale, Quaternion * outRotation)
{
	takesPosition.UpdateTake(frame, outPosition);
	takesScale.UpdateTake(frame, outScale);
	takesRotation.UpdateTake(frame, outRotation);
}

void AnimationTake::GetMaxTake(Vector3 * outPosition, Vector3 * outScale, Quaternion * outRotation)
{
	takesPosition.GetMaxTake(outPosition);
	takesScale.GetMaxTake(outScale);
	takesRotation.GetMaxTake(outRotation);
}

