using Play_Kazhutha.PlayLocal;

namespace Play_Kazhutha
{
    public partial class MainPage : ContentPage
    {
        int count = 0;

        public MainPage()
        {
            InitializeComponent();
        }

        private async void LocalButton(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new localJoin());
            //await Navigation.PushModalAsync(new LoginPage());
        }

        private async void OnlineButton(object sender, EventArgs e)
        {
            await DisplayAlert("My bad!!", "This Feature is Coming Soon....", "Skill Issyue");
            //await Navigation.PushAsync(new ());
            //await Navigation.PushModalAsync(new LoginPage());
        }
    }

}
