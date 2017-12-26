using System;

namespace CallbackSample
{
  /// <summary>
  /// The CallbackOption enumeration defines the demonstrations included in this sample.
  /// </summary>
  internal enum CallbackOption
  {
    UseInterface = 1,
    UseSynchronousDelegate,
    UseAsynchronousDelegateWithWait,
    UseAsynchronousDelegateWithCallback,
    UseFrameworkSuppliedAsynchronousMethodWithWait,
    UseFrameworkSuppliedAsynchronousMethodWithCallback,
    UseFrameworkSuppliedSynchronousMethod
  }
}