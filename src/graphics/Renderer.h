#pragma once
#include "Graphics.h"
#include "gui/game/RenderPreset.h"
#include "gui/interface/Point.h"
#include "common/tpt-rand.h"
#include "SimulationConfig.h"
#include "FindingElement.h"
#include <optional>
#include <array>
#include <memory>
#include <mutex>
#include <vector>

class RenderPreset;
class Simulation;
class Renderer;
struct Particle;

struct GraphicsFuncContext
{
	const Renderer *ren;
	const Simulation *sim;
	RNG rng;
	const Particle *pipeSubcallCpart;
	Particle *pipeSubcallTpart;
};

int HeatToColour(float temp);

class Renderer: public RasterDrawMethods<Renderer>
{
	using Video = PlaneAdapter<std::array<pixel, WINDOW.X * RES.Y>, WINDOW.X, RES.Y>;
	Video video;
	std::array<pixel, WINDOW.X * RES.Y> persistentVideo;
	Video warpVideo;

	Rect<int> GetClipRect() const
	{
		return video.Size().OriginRect();
	}

	friend struct RasterDrawMethods<Renderer>;

	float fireIntensity = 1;

public:
	Vec2<int> Size() const
	{
		return video.Size();
	}

	pixel const *Data() const
	{
		return video.data();
	}

	RNG rng;

	const Simulation *sim;

	std::vector<unsigned int> render_modes;
	unsigned int render_mode;
	unsigned int colour_mode;
	std::vector<unsigned int> display_modes;
	unsigned int display_mode;
	std::vector<RenderPreset> renderModePresets;
	//
	unsigned char fire_r[YCELLS][XCELLS];
	unsigned char fire_g[YCELLS][XCELLS];
	unsigned char fire_b[YCELLS][XCELLS];
	unsigned int fire_alpha[CELL*3][CELL*3];
	//
	bool gravityZonesEnabled;
	bool gravityFieldEnabled;
	int decorations_enable;
	bool blackDecorations;
	bool debugLines;
	pixel sampleColor;
	std::optional<FindingElement> findingElement;
	int foundElements;

	//Mouse position for debug information
	ui::Point mousePos;

	//Zoom window
	ui::Point zoomWindowPosition;
	ui::Point zoomScopePosition;
	int zoomScopeSize;
	bool zoomEnabled;
	int ZFACTOR;

	//Renderers
	void RenderBegin();
	void RenderEnd();

	void RenderZoom();
	void DrawBlob(Vec2<int> pos, RGB<uint8_t> colour);
	void DrawWalls();
	void DrawSigns();
	void render_gravlensing(const Video &source);
	void render_fire();
	float GetFireIntensity() const
	{
		return fireIntensity;
	}
	void prepare_alpha(int size, float intensity);
	void render_parts();
	void draw_grav_zones();
	void draw_air();
	void draw_grav();
	void draw_other();
	void FinaliseParts();

	void ClearAccumulation();
	void clearScreen();
	void SetSample(Vec2<int> pos);

	void draw_icon(int x, int y, Icon icon);

	VideoBuffer DumpFrame();

	pixel GetPixel(Vec2<int> pos) const;
	//...
	//Display mode modifiers
	void CompileDisplayMode();
	void CompileRenderMode();
	void AddRenderMode(unsigned int mode);
	void SetRenderMode(std::vector<unsigned int> render);
	std::vector<unsigned int> GetRenderMode();
	void RemoveRenderMode(unsigned int mode);
	void AddDisplayMode(unsigned int mode);
	void RemoveDisplayMode(unsigned int mode);
	void SetDisplayMode(std::vector<unsigned int> display);
	std::vector<unsigned int> GetDisplayMode();
	void SetColourMode(unsigned int mode);
	unsigned int GetColourMode();

	void ResetModes();

	int GetGridSize() { return gridSize; }
	void SetGridSize(int value) { gridSize = value; }

	static std::unique_ptr<VideoBuffer> WallIcon(int wallID, Vec2<int> size);

	Renderer(Simulation *newSim);

#define RENDERER_TABLE(name) \
	static std::vector<RGB<uint8_t>> name; \
	static inline RGB<uint8_t> name ## At(int index) \
	{ \
		auto size = int(name.size()); \
		if (index <        0) index =        0; \
		if (index > size - 1) index = size - 1; \
		return name[index]; \
	}
	RENDERER_TABLE(flameTable)
	RENDERER_TABLE(plasmaTable)
	RENDERER_TABLE(heatTable)
	RENDERER_TABLE(clfmTable)
	RENDERER_TABLE(firwTable)
#undef RENDERER_TABLE
	static void PopulateTables();

private:
	int gridSize;

	static constexpr Vec2<int> kitcatSpriteConfig { 3, 4 };
	static constexpr int kitcatSpriteSize = 13;
	static uint8_t SampleSpriteSheet(Vec2<int> spritePos, Vec2<int> samplePos, int scale);
	static PlaneAdapter<std::vector<uint8_t>> MakeMipMap(int scale, const char * filterName);
	static std::array<PlaneAdapter<std::vector<uint8_t>>, (kitcatSpriteSize * 2)-1> kitcatmipmap;
};
