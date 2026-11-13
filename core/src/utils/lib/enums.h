#pragma once

enum class NetworkMode
{
  Access,
  Station,
};

enum class ServerMode
{
  Developer,
  Production
};

enum class PowerState
{
  Idle,   // Landed and disarmed: automatic light sleep
  Flying, // Armed: control loop running, no light sleep
};
