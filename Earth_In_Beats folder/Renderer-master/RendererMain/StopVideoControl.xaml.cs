using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Display;
using Windows.System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace RendererMain
{
    public sealed partial class StopVideoControl : UserControl, IDisposable
    {
        private CoreIndependentInputSource coreInput = null;
        private bool disposed = false; // to detect redundant calls
        private StopNative.D3DView d3dView;

        public StopVideoControl()
        {
            this.InitializeComponent();

            StopNative.Class1 c = new StopNative.Class1();

            // create c++ component
            this.d3dView = new StopNative.D3DView();
            this.d3dView.Initialize(this.swapChainPanel);
            // create c++ component

            DisplayInformation displayInfo = DisplayInformation.GetForCurrentView();
            CoreWindow window = Window.Current.CoreWindow;

            window.VisibilityChanged += window_VisibilityChanged;

            displayInfo.DpiChanged += displayInfo_DpiChanged;
            displayInfo.OrientationChanged += displayInfo_OrientationChanged;
            DisplayInformation.DisplayContentsInvalidated += DisplayInformation_DisplayContentsInvalidated;

            this.swapChainPanel.CompositionScaleChanged += swapChainPanel_CompositionScaleChanged;
            this.swapChainPanel.SizeChanged += swapChainPanel_SizeChanged;

            Application.Current.Suspending += Current_Suspending;
            Application.Current.Resuming += Current_Resuming;

            var inputItemHandler = new WorkItemHandler((IAsyncAction inputAction) =>
            {
                this.coreInput = this.swapChainPanel.CreateCoreIndependentInputSource(
                    CoreInputDeviceTypes.Mouse |
                    CoreInputDeviceTypes.Pen |
                    CoreInputDeviceTypes.Touch);

                this.coreInput.PointerPressed += coreInput_PointerPressed;
                this.coreInput.PointerMoved += coreInput_PointerMoved;
                this.coreInput.PointerReleased += coreInput_PointerReleased;
                this.coreInput.PointerEntered += coreInput_PointerEntered;
                this.coreInput.PointerExited += coreInput_PointerExited;

                this.coreInput.Dispatcher.ProcessEvents(CoreProcessEventsOption.ProcessUntilQuit);
            });

            var inputTask = ThreadPool.RunAsync(inputItemHandler, WorkItemPriority.High, WorkItemOptions.TimeSliced);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    if (this.coreInput != null)
                    {
                        this.coreInput.Dispatcher.StopProcessEvents();
                    }
                }

                disposed = true;
            }
        }

        private void Current_Resuming(object sender, object e)
        {
            // load state
        }

        private void Current_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            // save state
        }

        private void coreInput_PointerExited(object sender, PointerEventArgs args)
        {
        }

        private void coreInput_PointerEntered(object sender, PointerEventArgs args)
        {
        }

        private void coreInput_PointerReleased(object sender, PointerEventArgs args)
        {
        }

        private void coreInput_PointerMoved(object sender, PointerEventArgs args)
        {
        }

        private void coreInput_PointerPressed(object sender, PointerEventArgs args)
        {
        }

        private void swapChainPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
        }

        private void swapChainPanel_CompositionScaleChanged(SwapChainPanel sender, object args)
        {
        }

        private void DisplayInformation_DisplayContentsInvalidated(DisplayInformation sender, object args)
        {
        }

        private void displayInfo_OrientationChanged(DisplayInformation sender, object args)
        {
        }

        private void displayInfo_DpiChanged(DisplayInformation sender, object args)
        {
        }

        private void window_VisibilityChanged(CoreWindow sender, VisibilityChangedEventArgs args)
        {
        }
    }
}
