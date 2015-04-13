using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using MusicMaper.Models;
using MusicMaper.Logic;

namespace MusicMaper.Controllers
{
    public class HomeController : Controller
    {
        TrackManager manager = new TrackManager();

        public ActionResult Index()
        {
            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Your application description page.";

            return View();
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";
            return View();
        }

        public ActionResult Update(string Data)
        {
            ViewBag.Result = "Wrong input! Must be: ?Data={PhoneStatus}?{PhoneID}?{Longitude}?{Latitude}?{Artist}?{Title}. Examlpe:www.sm.com/Home/Update/?Data=Play?PHONE_INDIFICATION?45.3?77.4?Metallica?Master";
            if (Data == null)
                return View();

            try
            {
                PhoneInput phone = PhoneInput.Parse(Data);

                manager.Update(phone);
                ViewBag.Result = "Ok";
                
            }
            catch (FormatException e)
            {
                ViewBag.Result = e.Message;
            }
            catch (Exception e)
            {
                ViewBag.Result = "Fail";
            }

            return View();
        }
        
    }
}