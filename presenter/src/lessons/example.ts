import {type LessonPlanType, LessonStateNames} from "../common/LessonStates"

const lesson: LessonPlanType = [
{
    "type": LessonStateNames.Introduction,
    "text": "Welcome to the Show"
},
{
    "type": LessonStateNames.Section,
    "text": "What is OpenClicker?",
    "photo": "https://upload.wikimedia.org/wikipedia/commons/4/46/Zenith_Space_Commander_600.jpg"
}
];

export default lesson;