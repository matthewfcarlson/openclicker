import { defineStore } from 'pinia'
const  ipcRenderer = require('electron').ipcRenderer;
import { LessonPlan, LessonPlanType } from '../../common/LessonStates';

export const useLessonStore = defineStore({
    id: 'lesson',
    state: () => ({
        count: 0,
        current_lesson: null as LessonPlanType | null,
    }),
    getters: {
        isReady: (state) => {return state.current_lesson != null},
    },
    actions: {
        increment() {
            this.count++
        },
        async attemptFileLoad(): Promise<string> {
            try {
                const lesson_json = (await ipcRenderer.invoke("dialog:openFile")) as (string|null)
                if (lesson_json == null) return "Did not select a file"
                const raw_json = JSON.parse(lesson_json);
                return this.attemptLessonLoad(raw_json);
            } 
            catch (e) {
                console.error(e);
                return "Failed to load file";
            }
        },
        async attemptLessonLoad(raw_lesson:any): Promise<string> {
            try {
                console.log(raw_lesson);
                const parsed_lesson_plan = LessonPlan.safeParse(raw_lesson);
                if (parsed_lesson_plan.success == true) {
                    this.current_lesson = parsed_lesson_plan.data;
                }
                else {
                    return parsed_lesson_plan.error.message
                }
            } 
            catch (e) {
                console.error(e);
                return "Failed to load file";
            }
        }
    }
})