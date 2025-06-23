#ifndef APP_HPP_
#define APP_HPP_

#include <memory>

#include <vendor/json/include/nlohmann/json.hpp>

#include "saura/core/os/os.hpp"
#include "saura/ui/ui.hpp"

namespace Saura
{

using json = nlohmann::json;

class App
{
  private:
	std::unique_ptr<UI> ui_ctx;
	std::unique_ptr<OS> os_ctx;
	bool				is_running;

  public:
	App ();

	void init ();
	void deinit ();
	void update ();
};

} // namespace Saura

#endif
