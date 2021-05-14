#pragma once

#include "Node.h"
#include <entt/entt.hpp>

namespace NodeFactory {

	int NextId() {
		static int id = 1;
		return id++;
	}

	std::string fn_signature(std::string name) {
		return "float " + name + "(vec3 pos)";
	}

	std::string fn_declaration(std::string signature) {
		return signature + ";\n";
	}

	std::string fn_implementation(std::string signature, std::string body) {
		return signature + " {\n" + body + "\n}\n";
	}

	inline entt::entity node(entt::registry& R, std::string name, CodeGenerator gen_source_code, UiWidgets show_widgets) {
		entt::entity e = R.create();
		std::string fn_name = name + "__" + std::to_string(static_cast<std::uint32_t>(e));
		R.emplace<Node>(e,
			/*name             */ name,
			/*fn_name          */ fn_name,
			/*node_id          */ static_cast<ed::NodeId>(NextId()),
			/*fn_declaration   */ fn_declaration(fn_signature(fn_name)),
			/*fn_implementation*/ "",
			/*gen_source_code  */ gen_source_code,
			/*show_widgets     */ show_widgets
		);
		R.emplace<IsTerminalNode>(e);
		return e;
	}

	inline entt::entity shape(entt::registry& R, std::string name, CodeGenerator fn_body_generator, UiWidgets show_widgets) {
		entt::entity e = node(R, name, fn_body_generator, show_widgets);
		R.emplace<ShapeNode>(e, OutputPinSingle(NextId()));
		return e;
	}

	inline entt::entity modifier(entt::registry& R, std::string name, CodeGenerator fn_body_generator, UiWidgets show_widgets) {
		entt::entity e = node(R, name, fn_body_generator, show_widgets);
		R.emplace<ModifierNode>(e,
			///*output_node*/ entt::null,
			/*input_pin  */ InputPinSingle(NextId()),
			/*output_pin */ OutputPinSingle(NextId())
		);
		return e;
	}

	struct Sphere {
		float radius;
	};

	inline entt::entity sphere(entt::registry& R) {
		entt::entity e = shape(R,
			"Sphere",
			[&](entt::entity e) {
				const auto& sphere = R.get<Sphere>(e);
				return "return length(pos) - " + std::to_string(sphere.radius) + ";";
			},
			[&](entt::entity e) {
				auto& sphere = R.get<Sphere>(e);
				ImGui::PushID(static_cast<int>(e));
				ImGui::SetNextItemWidth(200.f);
				bool b = ImGui::SliderFloat("radius", &sphere.radius, 0.f, 10.f);
				ImGui::PopID();
				return b;
			}
		);
		R.emplace<Sphere>(e, 1.f);
		return e;
	}

	inline entt::entity cube(entt::registry& R) {
		return shape(R,
			"Cube",
			[](entt::entity e) { return R"V0G0N( 
	vec3 q = abs(pos) - 1.; 
	return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.); 
	)V0G0N"; },
			[&](entt::entity e) {
				return false;
			}
		);
	}

	struct Transform {
		glm::vec3 pos;
	};

	inline entt::entity transform(entt::registry& R, NodeEditor& node_enditor) {
		entt::entity e = modifier(R,
			"Transform",
			[&](entt::entity e) {
				const auto& transfo = R.get<Transform>(e);
				entt::entity input_node = node_enditor.compute_node_connected_to_pin(R.get<ModifierNode>(e).input_pin.id);
				if (R.valid(input_node)) {
					std::string fn_name = R.get<Node>(input_node).fn_name;
					return "return " + fn_name + "(pos - vec3(" + std::to_string(transfo.pos.x) + ", " + std::to_string(transfo.pos.y) + ", " + std::to_string(transfo.pos.z) + "));";
				}
				else {
					return std::string("return MAX_DIST;");
				}
			},
			[&](entt::entity e) {
				auto& transfo = R.get<Transform>(e);
				ImGui::PushID(static_cast<int>(e));
				ImGui::SetNextItemWidth(200.f);
				bool b = ImGui::SliderFloat3("position", glm::value_ptr(transfo.pos), -10.f, 10.f);
				ImGui::PopID();
				return b;
			}
		);
		R.emplace<Transform>(e, glm::vec3(0.f));
		return e;
	}

} // namespace NodeFactory