
class FarmObserver {
 public:
  virtual void OnFlourReady() = 0;

 protected:
  virtual ~FarmObserver() = default;
};

class Farm {
 public:
  void AddObserver(FarmObserver* ob);
  void RemoveObserver(FarmObserver* ob);

 private:
  void NotifyFlourReady();
};

class Bakery : public FarmObserver {
 protected:
  void OnFlourReady() override;
};

int main() {
  Farm farm;
  Bakery bakery1, bakery2;

  farm.AddObserver(&bakery1);
  farm.AddObserver(&bakery2);

  // farm.OnWheatReady()
  // -> farm.ProduceFlourFromWheat()
  // -> farm.NotifyFlourReady()
  //    -> bakery1.OnFlourReady()
  //       -> bakery1.BuyFlourFromFarm()
  //       -> bakery1.ProduceBreadFromFlour()
  //       -> bakery1.ProduceCookieFromFlour()
  //    -> bakery2.OnFlourReady()
  //       -> bakery2.BuyFlourFromFarm()
  //       -> bakery2.ProduceCakeFromFlour()

  return 0;
}
