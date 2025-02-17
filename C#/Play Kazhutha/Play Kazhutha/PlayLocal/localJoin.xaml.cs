using Play_Kazhutha.PlayLocal.Host;

namespace Play_Kazhutha.PlayLocal;

public partial class localJoin : ContentPage
{
	public localJoin()
	{
		InitializeComponent();
	}
    private async void OnStartServerClicked(object sender, EventArgs e)
    {
        //await DisplayAlert("My bad!!", "This Feature is Coming Soon....", "Skill Issyue");
        await Navigation.PushAsync(new localHost());
        //await Navigation.PushModalAsync(new LoginPage());
    }
    public async void start()
	{
        Client client = new Client();
        await client.ConnectToServer("192.168.43.1", 7777);
        await client.SendMessage("Hello Server!");
        string mes = client.ReturnMessageTemp();
        await DisplayAlert("Message", mes, "ok");
    }
    private async void OnConnectClientClicked(object sender, EventArgs e)
    {
        start();
        //await DisplayAlert("My bad!!", "This Feature is Coming Soon....", "Skill Issyue");
        //await Navigation.PushAsync(new localHost());
        //await Navigation.PushModalAsync(new LoginPage());
    }
}