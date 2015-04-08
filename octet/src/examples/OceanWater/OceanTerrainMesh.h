

namespace octet
{
	class OceanTerrainMesh
	{
		public:

			struct water_geometry_source : mesh_terrain::geometry_source {
				mesh::vertex vertex(
					vec3_in bb_min, vec3_in uv_min, vec3_in uv_delta, vec3_in pos
					) 
				{
				
					float y = 0.0f;
					vec3 p = bb_min + pos + vec3(0, y, 0);
					vec3 normal = normalize(vec3(0, 1, 0));
					vec3 uv = uv_min + vec3((float)pos.x(), (float)pos.z(), 0) * uv_delta;
					return mesh::vertex(p, normal, uv);
				}
			};

			struct GerstnerWave
			{
			public:

				float wavelength;
				float amplitude;
				float speed;
				vec3 direction;
				float frequency;
				float phase;
				float steepness;

				GerstnerWave() = default;
				GerstnerWave(float w, float a, float s, vec3 d, float totSteep) : wavelength(w), amplitude(a), speed(s), direction(d)
				{
					frequency = 2 * (float)PI / wavelength;
					phase = speed * frequency;
					steepness = totSteep / (NUM_WAVES * frequency * amplitude);
				}

			public:

				float GetHeightPosition(float x, float z, float t)
				{
					float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
					return amplitude * sinf(displacement * frequency + phase * t);
				}

				vec3 GetPosition(float x, float z, float t)
				{
					vec3 pos(0.0f, 0.0f, 0.0f);
					float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
					pos.y() = amplitude * sinf(displacement * frequency + phase * t);
					pos.x() = steepness * amplitude * direction.x() * cosf(displacement * frequency + phase * t);
					pos.z() = steepness * amplitude * direction.z() * cosf(displacement * frequency + phase * t);
					return pos;
				}

				vec3 GetNormal(float x, float z, float t, vec3 point)
				{
					vec3 nor(0.0f, 0.0f, 0.0f);
					float displacement = direction.dot(point);
					nor.y() -= steepness * amplitude * frequency * sinf(frequency * displacement + phase * t);
					nor.x() -= direction.x() * amplitude * frequency * cosf(frequency * displacement + phase * t);
					nor.z() -= direction.z() * amplitude * frequency * cosf(frequency * displacement + phase * t);
					return nor;
				}

			};
	};
}


