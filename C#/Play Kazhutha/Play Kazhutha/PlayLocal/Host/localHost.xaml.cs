//using Android.App;

namespace Play_Kazhutha.PlayLocal.Host;

public partial class localHost : ContentPage
{
	public localHost()
	{
		InitializeComponent();
        start();
    }
    async void start()
    {
        Server server = new Server();
        await server.StartServer(7777);
        string mess = server.ReturnMessageTemp();
        await DisplayAlert("Message", mess, "ok");
    }
}