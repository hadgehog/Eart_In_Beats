using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace Renderer
{
    partial class MainPage : Page
    {
        RendererWinRT.Renderer renderer;

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            this.renderer = new RendererWinRT.Renderer();
            this.renderer.Initialize(this.swapChainPanel);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);

            this.renderer.Dispose();
        }
    }
}
